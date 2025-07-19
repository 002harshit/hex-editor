#define IVY_IMPL
#include "ivy/ivy.h"
#include "ivy/ivy_math.h"

#define RC_TGA_IMPL
#include "rc_tga.h"

#include <string.h>

typedef struct {
	int width, height;
	u32_t* data;
} sprite_t;

sprite_t font;
window_context_t wnd;

sprite_t load_sprite(const char* font_filepath)
{
	int x, y;
	u32_t* data = (u32_t*)rc_load_tga(font_filepath, &x, &y);
	return (sprite_t){
		.width = x, 
		.height = y,
		.data = data,
	};
}

void draw_sprite(sprite_t spr, int tx, int ty, int scale)
{
	int width = fmin(wnd.pixels.width, spr.width*scale);
	int height = fmin(wnd.pixels.height, spr.height*scale);
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			pixel_t color = spr.data[(y/scale) * spr.height + (x/scale)];
			if (!(color & 0xff000000)) {
				_gfx_set_pixel_unsafe(&wnd.pixels, x+tx, y+ty, color);
			}
		}
		
	}
}

// FONT 80x80 pixels
// START: ' ' END: '~'
void draw_string_unsafe(const char* buf, size_t buf_size, int font_scale, int tx, int ty, pixel_t color)
{
    buf_size = fmin(strlen(buf), buf_size);
    for (size_t b = 0; b < buf_size; b++) {
        char c = buf[b];  
        if (c < ' ' || c > '~') {
            c = '.';
        }
        int ox = (c - ' ') % 10;
        int oy = (c - ' ') / 10;
        int sx = tx + b * font_scale * 8;
        int sy = ty;
        for (int y = 0; y < 8; y++) {
            int py = sy + y * font_scale;
            for (int x = 0; x < 8; x++) {
                int px = sx + x * font_scale;
                pixel_t should_draw = font.data[(oy * 8 + y) * font.width + (ox * 8 + x)];
                if (!(should_draw & 0xff000000)) continue;
                for (int ry = py; ry < py + font_scale; ry++) {
                    for (int rx = px; rx < px + font_scale; rx++) {
                        gfx_set_pixel(&wnd.pixels, rx, ry, color);
                    }
                }
            }
        }        
    }
}

size_t file_size(FILE* fp)
{
	fseek(fp, 0, SEEK_END); 
	size_t s = ftell(fp);
	fseek(fp, 0, SEEK_SET); 
	return s;
}

size_t file_readall(const char* path, char** buf)
{
	FILE* fp = fopen(path, "r");
	if (!fp) {
		FATAL("Unable to open file");
	}
	size_t buf_size = file_size(fp);
	*buf = malloc(buf_size);
	fread(*buf, 1, buf_size, fp);
	fclose(fp);
	return buf_size;
}

void draw_rect(int rx, int ry, int width, int height, pixel_t color)
{
	width = fmin(wnd.pixels.width, width + rx);
	height = fmin(wnd.pixels.height, height + ry);
	for(int y = ry; y < height; y++)
		for(int x = rx; x < width; x++)
		    gfx_set_pixel(&wnd.pixels, x, y, color);
}

void draw_rect_line(int rx, int ry, int rw, int rh, pixel_t color)
{
	pixel_array_t ctx = wnd.pixels;
	// top
	for (int x = fmax(rx, 0); x < fmin(ctx.width, rx+rw); x++) {
		gfx_set_pixel(&ctx, x, ry, color);
	}
	// bottom
	for (int x = fmax(rx, 0); x < fmin(ctx.width, rx+rw); x++) {
		gfx_set_pixel(&ctx, x, ry + rh, color);	
	}

	// left
	for (int y = fmax(ry, 0); y < fmin(ctx.height, ry+rh); y++) {
		gfx_set_pixel(&ctx, rx, y, color);
	}
	// right
	for (int y = fmax(ry, 0); y < fmin(ctx.height, ry+rh); y++) {
		gfx_set_pixel(&ctx, rx + rw, y, color);
	}
}

int scroll_y = 0;
int n_line = 0;
int n_column = 0;
char* buf;
size_t buf_size;
size_t selected_index = 0;
int font_scale = 2;
int padding = 30;
#define ADDRESS_SIZE 8
int count_hex_per_line(float font_scale, float padding, float avaliable_width)
{
	int n_line = (avaliable_width - 4 * padding) / (24 * font_scale + padding);
	return n_line;
}

int count_hex_per_column(float font_scale, float padding, float avaliable_height)
{
	int n_column = (avaliable_height - padding * 2) / (font_scale * 8);
	return n_column;
}

void draw_address_space(int font_scale, int padding, int n_line)
{
	int addr_curr = scroll_y;
	char buf[ADDRESS_SIZE + 1];
	int x = padding;
	draw_rect(padding / 2, padding / 2, 8 * font_scale * ADDRESS_SIZE + padding, wnd.pixels.height - padding, 0x214421);
	for (int y = padding; y + font_scale * 8 < wnd.pixels.height; y += font_scale * 8) 
	{
		size_t buf_size = sprintf(buf, "%08x", addr_curr);
		addr_curr += n_line;
		draw_string_unsafe(buf, buf_size, font_scale, x, y, 0x00FF00);
	}
}

void draw_hex_space(int font_scale, int padding, char* hex, size_t hex_size, int n_line)
{
	char buf[8];
	size_t idx = scroll_y;
	int start_x = 2 * padding + (font_scale * 8 * 8);
	int x, y;
	int should_close = 0;
	draw_rect(start_x - padding / 2, padding / 2, (n_line * (font_scale * 16 + padding) + padding*2), wnd.pixels.height - padding, 0x00AAFF);
	for (y = padding; y + font_scale * 8 < wnd.pixels.height; y += font_scale * 8)
	{
		int num_drawn = 0;
		for (x = start_x; x + font_scale * 8 * 2 < wnd.pixels.width; x+= font_scale * 8 * 2 + padding)
		{
			int buf_size = sprintf(buf, "%02x", (u8_t)hex[idx]);
			if (idx == selected_index) {
				draw_string_unsafe(buf, buf_size, font_scale, x, y, 0x00FFFFF);
			} else {
				draw_string_unsafe(buf, buf_size, font_scale, x, y, 0x0000FF);
			}
			idx++;
			num_drawn++;
			
			if (num_drawn >= n_line) {
				break;
			}
			if (idx >= hex_size)
			{
				should_close = 1;
				break;
			}
		}
		if (should_close) 
			break;
	}
}

void draw_ascii_space(int font_scale, int padding, char* hex, size_t hex_size, int n_line)
{
	char buf[8];
	size_t idx = scroll_y;
	int start_x = 2 * padding + (font_scale * 8 * ADDRESS_SIZE) + (n_line * (font_scale * 16 + padding));
	int x, y;
	int should_close = 0;
	draw_rect(start_x - padding / 2, padding / 2, n_line * (font_scale * 8) + padding * 2, wnd.pixels.height - padding, 0x442121);
	for (y = padding; y + font_scale * 8 < wnd.pixels.height; y += font_scale * 8)
	{
		int num_drawn = 0;
		for (x = start_x; x + font_scale * 8 * 2 < wnd.pixels.width; x+= font_scale * 8)
		{
			int buf_size = sprintf(buf, "%c", (char)hex[idx]);
			if (idx == selected_index) {
				draw_string_unsafe(buf, buf_size, font_scale, x, y, 0xFFFFFF);
			} else {
				draw_string_unsafe(buf, buf_size, font_scale, x, y, 0xFF0000);
			}
			idx++;
			num_drawn++;
			
			if (num_drawn >= n_line) {
				break;
			}
			if (idx >= hex_size)
			{
				should_close = 1;
				break;
			}
		}
		if (should_close) 
			break;
	}
}

void draw_data_space(int font_scale, int padding, char c)
{
	int start_x = 2 * padding + (font_scale * 8 * 8) + (n_line * (font_scale * 16 + padding) + padding*2) + (n_line * (font_scale * 8) + padding * 2);
	char buf[32];
	int yc = 0, buf_size = -1;
	buf_size = sprintf(buf, "%d", (i8_t)c);
	draw_string_unsafe("8bit sint", -1, font_scale, start_x, (yc + 1) * padding + (yc * font_scale * 8), 0xFF0000); yc++;
	draw_string_unsafe(buf, buf_size, font_scale, start_x, (yc + 1) * padding + (yc * font_scale * 8), 0xFF0000); yc++;
	yc++;
	buf_size = sprintf(buf, "%u", (u8_t)c);
	draw_string_unsafe("8bit uint", -1, font_scale, start_x, (yc + 1) * padding + (yc * font_scale * 8), 0xFF0000); yc++;
	draw_string_unsafe(buf, buf_size, font_scale, start_x, (yc + 1) * padding + (yc * font_scale * 8), 0xFF0000); yc++;
	
	
}

void update_selected()
{
	int sx, sy, sw, sh, mx, my;
	sx = 2 * padding + (font_scale * 8 * 8);
	sy = padding;
	sw = n_line * (font_scale * 16 + padding) - padding;
	sh = wnd.pixels.height - padding*2;
	mx = wnd.mouse_x;
	my = wnd.mouse_y;
	printf("\n");
	INFO("MOUSE  %d %d", mx, my);
	INFO("BOUNDS %d %d %d %d", sx, sy, sw+sx, sh+sy);
	if (mx >= sx && mx < sx + sw && my >= sy && my < sy + sh) {
		INFO("WITHIN BOUNDS");
		int cx = (mx - sx) / (font_scale * 16 + padding / 2);
		int cy = (my - sy) / (font_scale * 8);
		selected_index = cy * n_line + cx + scroll_y;
		
		INFO("CELL %d %d, INDEX %zu", cx, cy, selected_index);
	}
}

void key_pressed_cb(struct window_context_t *_wnd, int key, int mods)
{
	switch(key) {
	case IVY_KEY_ESCAPE:
		_wnd->is_closed = 1;
		break;

	case IVY_KEY_BUTTON_1: {
		update_selected();
	} break;
	

	case IVY_KEY_PAGE_UP:
		scroll_y -= n_column * n_line;
		break;

	case IVY_KEY_PAGE_DOWN:
		scroll_y += n_column * n_line;
		break;
	
	case IVY_KEY_UP:
	case IVY_KEY_BUTTON_4:
		scroll_y -= n_line;
		break;

	case IVY_KEY_DOWN:
	case IVY_KEY_BUTTON_5:
		scroll_y += n_line;
		break;
	}
	// INFO("COLUMNS = %d", n_column);
	(void) mods;
}



int main(int argc, const char *argv[])
{
	if (argc < 2) {
		FATAL("Provide a input file");
	}
	buf_size = file_readall(argv[1], &buf);
	
	wnd = wnd_create(640, 480, "Hex Editor");
	wnd.on_key_press = key_pressed_cb;
	font = load_sprite("fonts.tga");
	
	while(!wnd_update(&wnd)) {
		int avail_width = wnd.pixels.width - (font_scale * 8 * ADDRESS_SIZE + padding * 2) - (font_scale * 8 * 8 + padding * 2);
		n_line = fmax(count_hex_per_line(font_scale, padding, avail_width), 1);
		n_column = count_hex_per_column(font_scale, padding, wnd.pixels.height);
		scroll_y = fmax(scroll_y, 0);
		scroll_y = fmin(scroll_y, buf_size);
		scroll_y = ((int)(scroll_y / n_line)) * n_line;
		draw_rect(0, 0, wnd.pixels.width, wnd.pixels.height, 0x101018);
		draw_address_space(font_scale, padding, n_line);
		draw_hex_space(font_scale, padding, buf, buf_size, n_line);
		draw_ascii_space(font_scale, padding, buf, buf_size, n_line);

		
		draw_data_space(font_scale, padding, buf[selected_index]);
	}
	wnd_destroy(&wnd);
}
