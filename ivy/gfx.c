#include "ivy.h"

pixel_array_t gfx_create(int width, int height)
{
	pixel_t *buffer = IVY_CALLOC(width * height, sizeof(pixel_t));
	if (!buffer) {
		FATAL("IVY GFX: Unable to allocate memory");
	}
	return (pixel_array_t){
		.width = width,
		.height = height,
		.max_size = width * height,
		.buffer = buffer,
	};
}

void gfx_resize(pixel_array_t *ctx, int width, int height)
{
	if (width * height > ctx->max_size) {
		pixel_t *buffer = IVY_CALLOC(width * height, sizeof(pixel_t));
		if (!buffer) {
			FATAL("IVY GFX: Unable to allocate memory");
		}
		free(ctx->buffer);
		ctx->buffer = buffer;
		ctx->max_size = width * height;
	}
	ctx->width = width;
	ctx->height = height;
}

void gfx_destroy(pixel_array_t *ctx)
{
	IVY_FREE(ctx->buffer);
	ctx->buffer = NULL;
	ctx->max_size = 0;
	ctx->width = 0;
	ctx->height = 0;
}
