#include "ivy.h"

static void default_mouse_move_cb(struct window_context_t *wnd, int mx, int my, int p_mx, int p_my)
{
	(void)wnd, (void)mx, (void)my, (void)p_mx, (void)p_my;
}

static void default_mouse_enter_cb(struct window_context_t *wnd, int mx, int my, int p_mx, int p_my)
{
	(void)wnd, (void)mx, (void)my, (void)p_mx, (void)p_my;
}

static void default_mouse_leave_cb(struct window_context_t *wnd, int mx, int my, int p_mx, int p_my)
{
	(void)wnd, (void)mx, (void)my, (void)p_mx, (void)p_my;
}

static void default_key_press_cb(struct window_context_t *wnd, int key, int mods)
{
	if (key == IVY_KEY_ESCAPE) {
		wnd->is_closed = 1;
	}
	(void)wnd, (void)key, (void)mods;
}

static void default_key_release_cb(struct window_context_t *wnd, int key, int mods)
{
	(void)wnd, (void)key, (void)mods;
}

static void default_text_input_cb(struct window_context_t *wnd, const char *buf, int buf_len)
{
	(void)wnd, (void)buf, (void)buf_len;
}

static void default_window_resize_cb(struct window_context_t *wnd, int w, int h)
{
	(void)wnd, (void)w, (void)h;
}

window_context_t wnd_create(u32_t width, u32_t height, const char *title)
{
	window_context_t wnd = {
		.first_mouse = 0,
		.is_closed = 0,
		.p_mouse_x = 0,
		.p_mouse_y = 0,
		.on_mouse_move = default_mouse_move_cb,
		.on_mouse_enter = default_mouse_enter_cb,
		.on_mouse_leave = default_mouse_leave_cb,
		.on_key_release = default_key_release_cb,
		.on_key_press = default_key_press_cb,
		.on_text_input = default_text_input_cb,
		.on_window_resize = default_window_resize_cb,
	};

	wnd.pixels = gfx_create(width, height);
	wnd.native = wnd_create_native(&wnd, width, height, title);

	if (!wnd.native) {
		FATAL("IVY_WND: Unable to create native window");
	}

	return wnd;
}

int wnd_update(window_context_t *wnd)
{
	if (wnd->is_closed) {
		return 1;
	}
	return wnd_update_native(wnd);
}

void wnd_destroy(window_context_t *wnd)
{
	wnd->is_closed = 1;
	gfx_destroy(&wnd->pixels);
	wnd_destroy_native(wnd);
}
