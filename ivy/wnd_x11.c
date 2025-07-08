#include "ivy.h"

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
	Display *dsp;
	Window wnd;
	GC gc;
	XImage *img;
	Atom wnd_close_atom;
} window_native_t;

 int _get_mapped_mods(uint32_t state);
 int _get_mapped_key(int key);

void* wnd_create_native(window_context_t *wnd, u32_t width, u32_t height, const char *title)
{
	window_native_t *native = IVY_MALLOC(sizeof(window_native_t));
	if (!native) {
		return NULL;
		WARN("IVY_X11: Unable to allocate memory");
	}

	native->dsp = XOpenDisplay(NULL);
	if (!native->dsp) {
		return NULL;
		WARN("IVY_X11: Unable to open display");
	}

	int screen = DefaultScreen(native->dsp);
	Window root_wnd = RootWindow(native->dsp, screen);

	int wx = 0;
	int wy = 0;

	native->wnd = XCreateSimpleWindow(native->dsp, root_wnd, wx, wy, width, height, 0, WhitePixel(native->dsp, screen), BlackPixel(native->dsp, screen));

	if (!native->wnd) {
		return NULL;
		WARN("IVY_X11: Unable to create window");
	}

	if (!XSetStandardProperties(native->dsp, native->wnd, title, "X11 Window", None, NULL, 0, NULL)) {
		return NULL;
		WARN("IVY_X11: Unable to set window properties");
	}

	native->gc = XCreateGC(native->dsp, native->wnd, 0, 0);
	if (!native->gc) {
		return NULL;
		WARN("IVY_X11: Unable to Create Graphics Context");
	}

	long event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | StructureNotifyMask;
	XSelectInput(native->dsp, native->wnd, event_mask);
	XMapRaised(native->dsp, native->wnd);

	XSync(native->dsp, native->wnd);

	Visual *visual = DefaultVisual(native->dsp, 0);
	native->img = XCreateImage(native->dsp, visual, 24, ZPixmap, 0, (char *)wnd->pixels.buffer, width, height, 32, 0);

	native->wnd_close_atom = XInternAtom(native->dsp, "WM_DELETE_WINDOW", 0);
	return native;			
}

int wnd_update_native(window_context_t *wnd)
{
	window_native_t* native = wnd->native;
	pixel_array_t* pixels = &wnd->pixels;
	XPutImage(native->dsp, native->wnd, native->gc, native->img, 0, 0, 0, 0, pixels->width, pixels->height);
	XFlush(native->dsp);
	XEvent ev;
	while (XPending(native->dsp)) {
		XNextEvent(native->dsp, &ev);
		switch (ev.type) {
		case KeyPress: {
			// On key press
			{
				wnd->mods = _get_mapped_mods(ev.xkey.state);
				int key = XkbKeycodeToKeysym(native->dsp, ev.xkey.keycode, 0, 0);
				key = _get_mapped_key(key);
				wnd->keys[key] = 1;
				wnd->on_key_press(wnd, key, wnd->mods);
			}
			// on Text input
			{
				char buf[64];
				u64_t sym;
				int len_read = XLookupString(&ev.xkey, buf, 64 - 1, &sym, NULL);
				buf[len_read] = '\0';
				INFO("%d, [%s], [%zu]", len_read, buf, sizeof(sym));
				wnd->on_text_input(wnd, buf, len_read);
			}
		} break;
		case KeyRelease: {
			wnd->mods = _get_mapped_mods(ev.xkey.state);
			int key = XkbKeycodeToKeysym(native->dsp, ev.xkey.keycode, 0, 0);
			key = _get_mapped_key(key);
			wnd->keys[key] = 0;
			wnd->on_key_release(wnd, key, wnd->mods);
		} break;
		case ButtonPress: {
			wnd->mods = _get_mapped_mods(ev.xbutton.state);
			int key = _get_mapped_key(ev.xbutton.button);
			wnd->keys[key] = 1;
			wnd->on_key_press(wnd, key, wnd->mods);
		} break;
		case ButtonRelease: {
			wnd->mods = _get_mapped_mods(ev.xbutton.state);
			int key = _get_mapped_key(ev.xbutton.button);
			wnd->keys[key] = 0;
			wnd->on_key_release(wnd, key, wnd->mods);
		} break;
		case MotionNotify: {
			if (wnd->first_mouse) {
				wnd->p_mouse_x = ev.xmotion.x;
				wnd->p_mouse_y = ev.xmotion.y;
			}
			wnd->mouse_x = ev.xmotion.x;
			wnd->mouse_y = ev.xmotion.y;
			wnd->on_mouse_move(wnd, ev.xmotion.x, ev.xmotion.y, wnd->p_mouse_x, wnd->p_mouse_y);
			wnd->p_mouse_x = ev.xmotion.x;
			wnd->p_mouse_y = ev.xmotion.y;
		} break;
		case EnterNotify: {
			wnd->p_mouse_x = wnd->mouse_x = ev.xcrossing.x;
			wnd->p_mouse_y = wnd->mouse_y = ev.xcrossing.y;
			wnd->on_mouse_enter(wnd, wnd->mouse_x, wnd->mouse_y, wnd->p_mouse_x, wnd->p_mouse_y);
		} break;
		case LeaveNotify: {
			wnd->p_mouse_x = wnd->mouse_x = ev.xcrossing.x;
			wnd->p_mouse_y = wnd->mouse_y = ev.xcrossing.y;
			wnd->on_mouse_leave(wnd, wnd->mouse_x, wnd->mouse_y, wnd->p_mouse_x, wnd->p_mouse_y);
		} break;
		case ConfigureNotify: {
			if (ev.xconfigure.window == native->wnd) {
				int width = ev.xconfigure.width;
				int height = ev.xconfigure.height;
				if (width != pixels->width || height != pixels->height) {
					
					gfx_resize(pixels, width, height);
					
					// XDestroyImage also frees the pixel buffer it stores
					// Thats why we have to manually free it like so
					IVY_FREE(native->img);
					
					Visual *visual = DefaultVisual(native->dsp, 0);
					native->img = XCreateImage(native->dsp, visual, 24, ZPixmap, 0, (char *)pixels->buffer, width, height, 32, 0);
					
					wnd->on_window_resize(wnd, width, height);
				}
			}
		} break;
		case Expose: {
			// no need to do it as we are redrawing every frame
			// XClearWindow(native->dsp, native->wnd);
		} break;
		case ClientMessage: {
			if ((Atom)ev.xclient.data.l[0] == native->wnd_close_atom) {
				wnd->is_closed = 1;
				return 1;
			}
		} break;
		default:
			 break;
		} // switch(ev.type)
	}
	return wnd->is_closed;
	
}

void wnd_destroy_native(window_context_t *wnd)
{
	window_native_t* native = wnd->native;
	IVY_FREE(native->img);
	IVY_FREE(native->gc);
	XCloseDisplay(native->dsp);
}


// clang-format off
int _get_mapped_key(int key)
{
	switch(key)
	{
		case Button1:			return IVY_KEY_BUTTON_1;
		case Button2:			return IVY_KEY_BUTTON_2;
		case Button3:			return IVY_KEY_BUTTON_3;
		case Button4:			return IVY_KEY_BUTTON_4;
		case Button5:			return IVY_KEY_BUTTON_5;

		case XK_Escape:         return IVY_KEY_ESCAPE;
		case XK_Tab:            return IVY_KEY_TAB;
		case XK_Shift_L:        return IVY_KEY_LEFT_SHIFT;
		case XK_Shift_R:        return IVY_KEY_RIGHT_SHIFT;
		case XK_Control_L:      return IVY_KEY_LEFT_CONTROL;
		case XK_Control_R:      return IVY_KEY_RIGHT_CONTROL;
		
		case XK_Meta_L:
		case XK_Alt_L:          return IVY_KEY_LEFT_ALT;
		
		case XK_Mode_switch:
		case XK_ISO_Level3_Shift:
		case XK_Meta_R:
		case XK_Alt_R:          return IVY_KEY_RIGHT_ALT;
		
		case XK_Super_L:        return IVY_KEY_LEFT_SUPER;
		case XK_Super_R:        return IVY_KEY_RIGHT_SUPER;
		case XK_Menu:           return IVY_KEY_MENU;
		case XK_Num_Lock:       return IVY_KEY_NUM_LOCK;
		case XK_Caps_Lock:      return IVY_KEY_CAPS_LOCK;
		case XK_Print:          return IVY_KEY_PRINT_SCREEN;
		case XK_Scroll_Lock:    return IVY_KEY_SCROLL_LOCK;
		case XK_Pause:          return IVY_KEY_PAUSE;
		case XK_Delete:         return IVY_KEY_DELETE;
		case XK_BackSpace:      return IVY_KEY_BACKSPACE;
		case XK_Return:         return IVY_KEY_ENTER;
		case XK_Home:           return IVY_KEY_HOME;
		case XK_End:            return IVY_KEY_END;
		case XK_Page_Up:        return IVY_KEY_PAGE_UP;
		case XK_Page_Down:      return IVY_KEY_PAGE_DOWN;
		case XK_Insert:         return IVY_KEY_INSERT;
		case XK_Left:           return IVY_KEY_LEFT;
		case XK_Right:          return IVY_KEY_RIGHT;
		case XK_Down:           return IVY_KEY_DOWN;
		case XK_Up:             return IVY_KEY_UP;
		case XK_F1:             return IVY_KEY_F1;
		case XK_F2:             return IVY_KEY_F2;
		case XK_F3:             return IVY_KEY_F3;
		case XK_F4:             return IVY_KEY_F4;
		case XK_F5:             return IVY_KEY_F5;
		case XK_F6:             return IVY_KEY_F6;
		case XK_F7:             return IVY_KEY_F7;
		case XK_F8:             return IVY_KEY_F8;
		case XK_F9:             return IVY_KEY_F9;
		case XK_F10:            return IVY_KEY_F10;
		case XK_F11:            return IVY_KEY_F11;
		case XK_F12:            return IVY_KEY_F12;
		case XK_F13:            return IVY_KEY_F13;
		case XK_F14:            return IVY_KEY_F14;
		case XK_F15:            return IVY_KEY_F15;
		case XK_F16:            return IVY_KEY_F16;
		case XK_F17:            return IVY_KEY_F17;
		case XK_F18:            return IVY_KEY_F18;
		case XK_F19:            return IVY_KEY_F19;
		case XK_F20:            return IVY_KEY_F20;
		case XK_F21:            return IVY_KEY_F21;
		case XK_F22:            return IVY_KEY_F22;
		case XK_F23:            return IVY_KEY_F23;
		case XK_F24:            return IVY_KEY_F24;
		case XK_F25:            return IVY_KEY_F25;

		case XK_KP_Divide:      return IVY_KEY_KP_DIVIDE;
		case XK_KP_Multiply:    return IVY_KEY_KP_MULTIPLY;
		case XK_KP_Subtract:    return IVY_KEY_KP_SUBTRACT;
		case XK_KP_Add:         return IVY_KEY_KP_ADD;

		case XK_KP_Insert:      return IVY_KEY_KP_0;
		case XK_KP_End:         return IVY_KEY_KP_1;
		case XK_KP_Down:        return IVY_KEY_KP_2;
		case XK_KP_Page_Down:   return IVY_KEY_KP_3;
		case XK_KP_Left:        return IVY_KEY_KP_4;
		case XK_KP_Right:       return IVY_KEY_KP_6;
		case XK_KP_Home:        return IVY_KEY_KP_7;
		case XK_KP_Up:          return IVY_KEY_KP_8;
		case XK_KP_Page_Up:     return IVY_KEY_KP_9;
		case XK_KP_Delete:      return IVY_KEY_KP_DECIMAL;
		case XK_KP_Equal:       return IVY_KEY_KP_EQUAL;
		case XK_KP_Enter:       return IVY_KEY_KP_ENTER;

		case XK_a:              return IVY_KEY_A;
		case XK_b:              return IVY_KEY_B;
		case XK_c:              return IVY_KEY_C;
		case XK_d:              return IVY_KEY_D;
		case XK_e:              return IVY_KEY_E;
		case XK_f:              return IVY_KEY_F;
		case XK_g:              return IVY_KEY_G;
		case XK_h:              return IVY_KEY_H;
		case XK_i:              return IVY_KEY_I;
		case XK_j:              return IVY_KEY_J;
		case XK_k:              return IVY_KEY_K;
		case XK_l:              return IVY_KEY_L;
		case XK_m:              return IVY_KEY_M;
		case XK_n:              return IVY_KEY_N;
		case XK_o:              return IVY_KEY_O;
		case XK_p:              return IVY_KEY_P;
		case XK_q:              return IVY_KEY_Q;
		case XK_r:              return IVY_KEY_R;
		case XK_s:              return IVY_KEY_S;
		case XK_t:              return IVY_KEY_T;
		case XK_u:              return IVY_KEY_U;
		case XK_v:              return IVY_KEY_V;
		case XK_w:              return IVY_KEY_W;
		case XK_x:              return IVY_KEY_X;
		case XK_y:              return IVY_KEY_Y;
		case XK_z:              return IVY_KEY_Z;
		case XK_1:              return IVY_KEY_1;
		case XK_2:              return IVY_KEY_2;
		case XK_3:              return IVY_KEY_3;
		case XK_4:              return IVY_KEY_4;
		case XK_5:              return IVY_KEY_5;
		case XK_6:              return IVY_KEY_6;
		case XK_7:              return IVY_KEY_7;
		case XK_8:              return IVY_KEY_8;
		case XK_9:              return IVY_KEY_9;
		case XK_0:              return IVY_KEY_0;
		case XK_space:          return IVY_KEY_SPACE;
		case XK_minus:          return IVY_KEY_MINUS;
		case XK_equal:          return IVY_KEY_EQUAL;
		case XK_bracketleft:    return IVY_KEY_LEFT_BRACKET;
		case XK_bracketright:   return IVY_KEY_RIGHT_BRACKET;
		case XK_backslash:      return IVY_KEY_BACKSLASH;
		case XK_semicolon:      return IVY_KEY_SEMICOLON;
		case XK_apostrophe:     return IVY_KEY_APOSTROPHE;
		case XK_grave:          return IVY_KEY_GRAVE_ACCENT;
		case XK_comma:          return IVY_KEY_COMMA;
		case XK_period:         return IVY_KEY_PERIOD;
		case XK_slash:          return IVY_KEY_SLASH;
		case XK_less:           return IVY_KEY_WORLD_1;
		default: 				return IVY_KEY_INVALID;
	}
}
// clang-format on

int _get_mapped_mods(uint32_t state)
{
	int mods = 0;
	if (state & ShiftMask)
		mods |= IVY_MOD_MASK_SHIFT;
	if (state & ControlMask)
		mods |= IVY_MOD_MASK_CONTROL;
	if (state & Mod1Mask)
		mods |= IVY_MOD_MASK_ALT;
	if (state & Mod4Mask)
		mods |= IVY_MOD_MASK_SUPER;
	if (state & LockMask)
		mods |= IVY_MOD_MASK_CAPS_LOCK;
	if (state & Mod2Mask)
		mods |= IVY_MOD_MASK_NUM_LOCK;
	return mods;
}
