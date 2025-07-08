#include <string.h>
#include "ivy/ivy.h"
#include "ivy/math.h"
#define RC_TGA_IMPL
#include "rc_tga.h"

typedef struct {
	int width, height;
	u32_t* data;
} sprite_t;

void draw_rect(window_context_t* wnd,int rx, int ry, int width, int height, pixel_t color)
{
	width = fmin(wnd->pixels.width, width + rx);
	height = fmin(wnd->pixels.height, height + ry);
	for(int y = ry; y < height; y++)
		for(int x = rx; x < width; x++)
			gfx_set_pixel(&wnd->pixels, x, y, color);
}

sprite_t load_sprite(const char* font_filepath)
{
	int x, y;
	u32_t* data = (u32_t*)rc_load_tga(font_filepath, &x, &y);
	// u32_t* data = (u32_t*)stbi_load(font_filepath, &x, &y, &channels, 4);
	INFO("x: %d, y: %d", x, y);
	return (sprite_t){
		.width = x, 
		.height = y,
		.data = data,
	};
}

void draw_sprite(window_context_t* wnd, sprite_t spr)
{
	int font_size = 2;
	int width = fmin(wnd->pixels.width, spr.width*font_size);
	int height = fmin(wnd->pixels.height, spr.height*font_size);
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			pixel_t color = spr.data[(y/font_size) * spr.height + (x/font_size)];
			if (color)
			{
				gfx_set_pixel(&wnd->pixels, x, y, 0xFFFFFF);
			}
		}
		
	}
}

// FONT 80x80 pixels
// START: ' ' END: '~'

void draw_char(window_context_t* wnd, sprite_t* font, char c, int px, int py, int font_scale)
{
	//0, 0 -> 0 ;	0, 1 -> 0; 0, 3 -> 1
	
	if (c < ' ' || c > '~') return;
	int offset_x = (c - ' ') % 10;
	int offset_y = (c - ' ') / 10;
	
	
	// INFO("%d, %d, %c", offset_x, offset_y, c);
	// hardcoding some values
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			pixel_t color = font->data[(offset_y * 8 + y) * font->width + (offset_x * 8 + x)];
			if (color) {
				draw_rect(wnd, px + x * font_scale, py + y * font_scale, font_scale, font_scale, 0xFFFFFF);
				
				// ivy_wnd_set_pixel(wnd, px + x * font_scale , py + y * font_scale, 0xFFFFFF);
			}
		}
	}
	
	
}


void draw_string(window_context_t* wnd, sprite_t* font)
{
	const char* str = "Hex Editor";
	int str_size = strlen(str);
	int font_scale = 8;
	for (int i = 0; i < str_size; i++)
	{
		char c = str[i];
		draw_char(wnd, font, c, i * 8 * font_scale, 10, font_scale);
	}
}

int main() {
	window_context_t wnd = wnd_create(640, 480, "Hex Editor");
	sprite_t font = load_sprite("fonts.tga");
	while(!wnd_update(&wnd)) {
		draw_rect(&wnd, 0, 0, wnd.pixels.width, wnd.pixels.height, 0x101018);
		draw_string(&wnd, &font);
	}
	wnd_destroy(&wnd);
}
