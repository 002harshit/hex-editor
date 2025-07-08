#ifndef IVY_H
#define IVY_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// ---------------------------------------------------------------
// IVY MACROS
// ---------------------------------------------------------------

#ifndef IVY_COLOR
#define IVY_COLOR

#define FG_BLACK "\033[0;30m"
#define FG_RED "\033[0;31m"
#define FG_GREEN "\033[0;32m"
#define FG_YELLOW "\033[0;33m"
#define FG_BLUE "\033[0;34m"
#define FG_PURPLE "\033[0;35m"
#define FG_CYAN "\033[0;36m"
#define FG_WHITE "\033[0;37m"

#define BOLD_BLACK "\033[1;30m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_PURPLE "\033[1;35m"
#define BOLD_CYAN "\033[1;36m"
#define BOLD_WHITE "\033[1;37m"

#define UNDERLINE_BLACK "\033[4;30m"
#define UNDERLINE_RED "\033[4;31m"
#define UNDERLINE_GREEN "\033[4;32m"
#define UNDERLINE_YELLOW "\033[4;33m"
#define UNDERLINE_BLUE "\033[4;34m"
#define UNDERLINE_PURPLE "\033[4;35m"
#define UNDERLINE_CYAN "\033[4;36m"
#define UNDERLINE_WHITE "\033[4;37m"

#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_PURPLE "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"

#define RESET "\033[0m"

#endif // IVY_COLOR

#ifndef IVY_MALLOC
#define IVY_MALLOC(s) malloc(s)
#endif // IVY_MALLOC

#ifndef IVY_CALLOC
#define IVY_CALLOC(num, s) calloc(num, s)
#endif // IVY_CALLOC

#ifndef IVY_FREE
#define IVY_FREE(s) free(s)
#endif // IVY_FREE

#ifndef IVY_FPRINTF
#ifdef IVY_NO_LOGGING
#define IVY_FPRINTF(...) \
	do {                 \
		break;           \
	} while (0)
#else
#define IVY_FPRINTF(std, ...) fprintf(std, __VA_ARGS__)
#endif // IVY_NO_LOGGING
#endif // IVY_FPRINTF

#ifndef WARN
#define WARN(...)                                                    \
	IVY_FPRINTF(stderr, FG_YELLOW "WARNING: " FG_WHITE __VA_ARGS__); \
	IVY_FPRINTF(stderr, RESET "\n")
#endif // WARN

#ifndef FATAL
#define FATAL(...)                                              \
	IVY_FPRINTF(stderr, FG_RED "FATAL: " FG_WHITE __VA_ARGS__); \
	IVY_FPRINTF(stderr, RESET "\n");                            \
	exit(1)
#endif // FATAL

#ifndef INFO
#define INFO(...)                                                \
	IVY_FPRINTF(stdout, FG_GREEN "INFO: " FG_WHITE __VA_ARGS__); \
	IVY_FPRINTF(stdout, RESET "\n")
#endif // INFO

#ifndef TODO
#define TODO(name)                                                                        \
	IVY_FPRINTF(stderr, FG_RED "%s:%d TODO: " FG_WHITE "%s\n", __FILE__, __LINE__, name); \
	exit(1)
#endif // TODO

#ifndef ASSERT
#ifndef IVY_NO_ASSERT
#define ASSERT(n)                                                                                                        \
	do {                                                                                                                 \
		if (!(n)) {                                                                                                      \
			IVY_FPRINTF(stderr, FG_YELLOW "%s:%d " FG_RED "ASSERTION FAILED: " FG_WHITE "%s\n", __FILE__, __LINE__, #n); \
			exit(1);                                                                                                     \
		}                                                                                                                \
		break;                                                                                                           \
	} while (0)
#else
#define ASSERT(n) \
	do {          \
		break;    \
	} while (0)
#endif // IVY_NO_ASSERT
#endif // ASSERT

// source:
// https://github.com/ramdeoshubham/macros/blob/c37a57f102921ab7abb8c50a093dfc007049dc53/macros.h#L128

#ifndef DEFER
#define DEFER_2(head, tail, i) for (int i = (head, 0); !i; tail, i++)
#define DEFER(head, tail) DEFER_2(head, tail, __deferVar__##__COUNTER__)
#endif // DEFER

#ifdef __cplusplus
#define EXTERN_C_START extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_START
#define EXTERN_C_END
#endif

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif // PI

#ifndef EPSILON
#define EPSILON 0.00001
#endif // EPSILON

// ---------------------------------------------------------------
// STRUCTURE DEFINATIONS
// ---------------------------------------------------------------

#ifndef IVY_COMMON_TYPES_T
#define IVY_COMMON_TYPES_T
typedef bool bool_t;
typedef unsigned char u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef uint64_t u64_t;
typedef signed char i8_t;
typedef int16_t i16_t;
typedef int32_t i32_t;
typedef int64_t i64_t;
#endif // IVY_COMMON_TYPES_T

// IVY MATH STRUCTS
#ifndef IVY_MATH_VEC2_T
#define IVY_MATH_VEC2_T

typedef struct {
	float x, y;
} vec2_t;
#endif // IVY_MATH_VEC2_T

#ifndef IVY_MATH_VEC3_T
#define IVY_MATH_VEC3_T

typedef struct {
	float x, y, z;
} vec3_t;
#endif // IVY_MATH_VEC3_T

#ifndef IVY_MATH_MAT4_T
#define IVY_MATH_MAT4_T

// Row Major Matrix
typedef struct {
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;
} mat4_t;
#endif // IVY_MATH_MAT4_T

// IVY STL LOADER STRUCT

typedef struct {
	vec3_t normal;
	vec3_t vertex1;
	vec3_t vertex2;
	vec3_t vertex3;
} stl_face_t;

typedef struct {
	size_t triangles_count;
	stl_face_t *triangles;
} stl_data_t;

// IVY AUDIO STRUCTS

typedef struct {
	void *native;
} audio_device_t;

// IVY GFX STRUCTS
typedef uint32_t pixel_t;

typedef struct {
	int width;
	int height;
	int max_size;
	pixel_t *buffer;
} pixel_array_t;

// IVY WND STRUCT
typedef enum {
	IVY_MOD_MASK_SHIFT = (1 << 0),
	IVY_MOD_MASK_CONTROL = (1 << 1),
	IVY_MOD_MASK_ALT = (1 << 2),
	IVY_MOD_MASK_SUPER = (1 << 3),
	IVY_MOD_MASK_CAPS_LOCK = (1 << 4),
	IVY_MOD_MASK_NUM_LOCK = (1 << 5),
} IVY_MOD_MASK;

typedef enum {
	IVY_KEY_FIRST = 0,
	IVY_KEY_BUTTON_1 = IVY_KEY_FIRST,
	IVY_KEY_BUTTON_2,
	IVY_KEY_BUTTON_3,
	IVY_KEY_BUTTON_4,
	IVY_KEY_BUTTON_5,

	IVY_KEY_ESCAPE,
	IVY_KEY_TAB,
	IVY_KEY_LEFT_SHIFT,
	IVY_KEY_RIGHT_SHIFT,
	IVY_KEY_LEFT_CONTROL,
	IVY_KEY_RIGHT_CONTROL,
	IVY_KEY_LEFT_ALT,
	IVY_KEY_RIGHT_ALT,
	IVY_KEY_LEFT_SUPER,
	IVY_KEY_RIGHT_SUPER,
	IVY_KEY_MENU,
	IVY_KEY_NUM_LOCK,
	IVY_KEY_CAPS_LOCK,
	IVY_KEY_PRINT_SCREEN,
	IVY_KEY_SCROLL_LOCK,
	IVY_KEY_PAUSE,
	IVY_KEY_DELETE,
	IVY_KEY_BACKSPACE,
	IVY_KEY_ENTER,
	IVY_KEY_HOME,
	IVY_KEY_END,
	IVY_KEY_PAGE_UP,
	IVY_KEY_PAGE_DOWN,
	IVY_KEY_INSERT,
	IVY_KEY_LEFT,
	IVY_KEY_RIGHT,
	IVY_KEY_DOWN,
	IVY_KEY_UP,
	IVY_KEY_F1,
	IVY_KEY_F2,
	IVY_KEY_F3,
	IVY_KEY_F4,
	IVY_KEY_F5,
	IVY_KEY_F6,
	IVY_KEY_F7,
	IVY_KEY_F8,
	IVY_KEY_F9,
	IVY_KEY_F10,
	IVY_KEY_F11,
	IVY_KEY_F12,
	IVY_KEY_F13,
	IVY_KEY_F14,
	IVY_KEY_F15,
	IVY_KEY_F16,
	IVY_KEY_F17,
	IVY_KEY_F18,
	IVY_KEY_F19,
	IVY_KEY_F20,
	IVY_KEY_F21,
	IVY_KEY_F22,
	IVY_KEY_F23,
	IVY_KEY_F24,
	IVY_KEY_F25,

	IVY_KEY_KP_DIVIDE,
	IVY_KEY_KP_MULTIPLY,
	IVY_KEY_KP_SUBTRACT,
	IVY_KEY_KP_ADD,

	IVY_KEY_KP_0,
	IVY_KEY_KP_1,
	IVY_KEY_KP_2,
	IVY_KEY_KP_3,
	IVY_KEY_KP_4,
	IVY_KEY_KP_6,
	IVY_KEY_KP_7,
	IVY_KEY_KP_8,
	IVY_KEY_KP_9,
	IVY_KEY_KP_DECIMAL,
	IVY_KEY_KP_EQUAL,
	IVY_KEY_KP_ENTER,
	IVY_KEY_A,
	IVY_KEY_B,
	IVY_KEY_C,
	IVY_KEY_D,
	IVY_KEY_E,
	IVY_KEY_F,
	IVY_KEY_G,
	IVY_KEY_H,
	IVY_KEY_I,
	IVY_KEY_J,
	IVY_KEY_K,
	IVY_KEY_L,
	IVY_KEY_M,
	IVY_KEY_N,
	IVY_KEY_O,
	IVY_KEY_P,
	IVY_KEY_Q,
	IVY_KEY_R,
	IVY_KEY_S,
	IVY_KEY_T,
	IVY_KEY_U,
	IVY_KEY_V,
	IVY_KEY_W,
	IVY_KEY_X,
	IVY_KEY_Y,
	IVY_KEY_Z,
	IVY_KEY_1,
	IVY_KEY_2,
	IVY_KEY_3,
	IVY_KEY_4,
	IVY_KEY_5,
	IVY_KEY_6,
	IVY_KEY_7,
	IVY_KEY_8,
	IVY_KEY_9,
	IVY_KEY_0,
	IVY_KEY_SPACE,
	IVY_KEY_MINUS,
	IVY_KEY_EQUAL,
	IVY_KEY_LEFT_BRACKET,
	IVY_KEY_RIGHT_BRACKET,
	IVY_KEY_BACKSLASH,
	IVY_KEY_SEMICOLON,
	IVY_KEY_APOSTROPHE,
	IVY_KEY_GRAVE_ACCENT,
	IVY_KEY_COMMA,
	IVY_KEY_PERIOD,
	IVY_KEY_SLASH,
	IVY_KEY_WORLD_1,
	IVY_KEY_LAST = IVY_KEY_WORLD_1,
	IVY_KEY_INVALID
} IVY_KEY;

typedef struct window_context_t window_context_t;

struct window_context_t {
	pixel_array_t pixels;

	bool_t first_mouse;
	bool_t is_closed;
	int mouse_x, mouse_y;
	int p_mouse_x, p_mouse_y;
	int keys[IVY_KEY_LAST + 1];
	int mods;

	void (*on_mouse_move)(window_context_t *wnd, int mx, int my, int pmx, int pmy);
	void (*on_mouse_enter)(window_context_t *wnd, int mx, int my, int pmx, int pmy);
	void (*on_mouse_leave)(window_context_t *wnd, int mx, int my, int pmx, int pmy);
	void (*on_key_release)(window_context_t *wnd, int key, int mod);
	void (*on_key_press)(window_context_t *wnd, int key, int mod);
	void (*on_text_input)(window_context_t *wnd, const char *buf, int buf_size);
	void (*on_window_resize)(window_context_t *wnd, int width, int height);

	void *native;
};

// ---------------------------------------------------------------
// GLOBAL FUNCTIONS DEFINATIONS
// ---------------------------------------------------------------
#ifndef IVY_GLOBAL_API
#define IVY_GLOBAL_API extern
#endif
#ifndef IVY_INLINE_API
#define IVY_INLINE_API static inline
#endif

EXTERN_C_START

// IVY STL
IVY_GLOBAL_API stl_data_t stl_load(const char *stl_filepath);
IVY_GLOBAL_API void stl_free(stl_data_t stl_data);

// IVY AUDIO
IVY_GLOBAL_API audio_device_t audio_open(unsigned int channels, unsigned int sample_rate, float latency_secs);
IVY_GLOBAL_API int audio_avail_frames(audio_device_t *audio);
IVY_GLOBAL_API void audio_write_buffer(audio_device_t *audio, float *buf, size_t buf_size);
IVY_GLOBAL_API void audio_close(audio_device_t *audio);

// IVY GFX
IVY_INLINE_API void _gfx_set_pixel_unsafe(pixel_array_t *ctx, int x, int y, pixel_t p)
{
	ctx->buffer[y * ctx->width + x] = p;
}

IVY_INLINE_API pixel_t _gfx_get_pixel_unsafe(pixel_array_t *ctx, int x, int y)
{
	return ctx->buffer[y * ctx->width + x];
}

IVY_INLINE_API void gfx_set_pixel(pixel_array_t *ctx, int x, int y, pixel_t p)
{
	if (x > -1 && x < ctx->width && y > -1 && y < ctx->height) {
		ctx->buffer[y * ctx->width + x] = p;
	}
}

IVY_INLINE_API pixel_t gfx_get_pixel(pixel_array_t *ctx, int x, int y)
{
	if (x > -1 && x < ctx->width && y > -1 && y < ctx->height) {
		return ctx->buffer[y * ctx->width + x];
	}
	return 0;
}

IVY_GLOBAL_API pixel_array_t gfx_create(int width, int height);
IVY_GLOBAL_API void gfx_resize(pixel_array_t *ctx, int width, int height);
IVY_GLOBAL_API void gfx_destroy(pixel_array_t *ctx);
IVY_GLOBAL_API void gfx_draw_rect(pixel_array_t *ctx, int rx, int ry, int rw, int rh);
IVY_GLOBAL_API void gfx_draw_line(pixel_array_t *ctx, int sx, int sy, int ex, int ey);
IVY_GLOBAL_API void gfx_draw_triangle(pixel_array_t *ctx, int sx, int sy, int ex, int ey);
// TODO: gfx_rasterize

// IVY WND
IVY_GLOBAL_API window_context_t wnd_create(u32_t width, u32_t height, const char *title);
IVY_GLOBAL_API int wnd_update(window_context_t *wnd);
IVY_GLOBAL_API void wnd_destroy(window_context_t *wnd);

// IVY WND NATIVE
IVY_GLOBAL_API void *wnd_create_native(window_context_t *wnd, u32_t width, u32_t height, const char *title);
IVY_GLOBAL_API int wnd_update_native(window_context_t *wnd);
IVY_GLOBAL_API void wnd_destroy_native(window_context_t *wnd);

EXTERN_C_END

#undef IVY_GLOBAL_API
#undef IVY_INLINE_API

#endif // IVY_H
