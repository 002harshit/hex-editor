#ifndef IVY_MATH_H
#define IVY_MATH_H

#include <math.h>

// ---------------------------------------------------------------------------
// IVY MATH DEFINATIONS
// ---------------------------------------------------------------------------
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif // PI

#ifndef EPSILON
#define EPSILON 0.00001
#endif // EPSILON

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

// ---------------------------------------------------------------------------
// IVY MATH IMPLEMENTATION
// ---------------------------------------------------------------------------

// TODO: implement vec2, vec3, mat4 printf
// ---------------------------------------------------------------------------
// LOGGING

// ---------------------------------------------------------------------------
// FLOAT

static inline float fsq(float f)
{
	return f * f;
}

static inline float deg_to_rad(float angle_rad)
{
	return angle_rad * PI / 180.0;
}

static inline float fwrap(float value, float min, float max)
{
	return value - (max - min) * floorf((value - min) / (max - min));
}

static inline int fequal(float x, float y)
{
	return (fabsf(x - y)) <= (EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))));
}

static inline float fclamp(float value, float min, float max)
{
	float res = (value < min) ? min : value;
	return (res > max) ? max : res;
}

static inline float flerp(float a, float b, float t)
{
	return a + t * (b - a);
}

static inline float fnormalize(float value, float min, float max)
{
	return (value - min) / (max - min);
}

static inline float fremap(float value, float smin, float smax, float emin, float emax)
{
	return emin + (value - smin) * (emax - emin) / (smax - smin);
}

// ---------------------------------------------------------------------------
// VECTOR2

static inline vec2_t vec2(float x, float y)
{
	return (vec2_t){x, y};
}

static inline vec2_t vec2_zero()
{
	return (vec2_t){0, 0};
}

static inline vec2_t vec2_add(vec2_t a, vec2_t b)
{
	return (vec2_t){a.x + b.x, a.y + b.y};
}

static inline vec2_t vec2_sub(vec2_t a, vec2_t b)
{
	return (vec2_t){a.x - b.x, a.y - b.y};
}

static inline vec2_t vec2_mul(vec2_t a, vec2_t b)
{
	return (vec2_t){a.x * b.x, a.y * b.y};
}

static inline vec2_t vec2_addv(vec2_t a, float v)
{
	return (vec2_t){a.x + v, a.y + v};
}

static inline vec2_t vec2_subv(vec2_t a, float v)
{
	return (vec2_t){a.x - v, a.y - v};
}

static inline vec2_t vec2_mulv(vec2_t a, float v)
{
	return (vec2_t){a.x * v, a.y * v};
}

static inline vec2_t vec2_divv(vec2_t a, float v)
{
	return (vec2_t){a.x / v, a.y / v};
}

static inline float vec2_lensq(vec2_t a)
{
	return fsq(a.x) + fsq(a.y);
}

static inline float vec2_len(vec2_t a)
{
	return sqrtf(fsq(a.x)) + (fsq(a.y));
}

static inline float vec2_distsq(vec2_t a, vec2_t b)
{
	return fsq(a.x - b.x) + fsq(a.y - b.y);
}

static inline float vec2_dist(vec2_t a, vec2_t b)
{
	return sqrtf(fsq(a.x - b.x) + fsq(a.y - b.y));
}

static inline vec2_t vec2_wrap(vec2_t value, vec2_t min, vec2_t max)
{
	return (vec2_t){fwrap(value.x, min.x, max.x), fwrap(value.y, min.y, max.y)};
}

static inline vec2_t vec2_wrap2(vec2_t value, float min, float max)
{
	return (vec2_t){fwrap(value.x, min, max), fwrap(value.y, min, max)};
}

static inline int vec2_is_almost_eq(vec2_t a, vec2_t b)
{
	return fequal(a.x, b.x) && fequal(a.y, b.y);
}

static inline vec2_t vec2_clamp(vec2_t value, vec2_t min, vec2_t max)
{
	return (vec2_t){fclamp(value.x, min.x, max.x), fclamp(value.y, min.y, max.y)};
}

static inline vec2_t vec2_clamp2(vec2_t value, float min, float max)
{
	return (vec2_t){fclamp(value.x, min, max), fwrap(value.y, min, max)};
}

static inline vec2_t vec2_lerp(vec2_t a, vec2_t b, float t)
{
	return (vec2_t){flerp(a.x, b.x, t), flerp(a.y, b.y, t)};
}

static inline vec2_t vec2_normalize(vec2_t a)
{
	float l = vec2_len(a);
	if (l > 0)
		return (vec2_t){a.x / l, a.y / l};
	return vec2_zero();
}

static inline vec2_t vec2_invert(vec2_t a)
{
	return (vec2_t){1.0 / a.x, 1.0 / a.y};
}

static inline float vec2_dot(vec2_t a, vec2_t b)
{
	return a.x * b.x + a.y * b.y;
}

static inline float vec2_cross(vec2_t a, vec2_t b)
{
	return a.x * b.y - a.y * b.x;
}

static inline float vec2_angle(vec2_t a, vec2_t b)
{
	return atan2f(vec2_cross(a, b), vec2_dot(a, b));
}

static inline vec2_t vec2_rotate(vec2_t a, float angle_rad)
{
	float c = cosf(angle_rad);
	float s = sinf(angle_rad);
	return (vec2_t){a.x * c - a.y * s, a.x * s + a.y * c};
}

// TODO: Reflect, refract, transform

// ---------------------------------------------------------------------------
// VECTOR3
static inline vec3_t vec3_zero()
{
	return (vec3_t){0, 0, 0};
}

static inline vec3_t vec3(float x, float y, float z)
{
	return (vec3_t){x, y, z};
}

static inline vec3_t vec3_add(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline vec3_t vec3_sub(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline vec3_t vec3_mul(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x * b.x, a.y * b.y, a.z * b.z};
}

static inline vec3_t vec3_div(vec3_t a, vec3_t b)
{
	return (vec3_t){a.x / b.x, a.y / b.y, a.z / b.z};
}

static inline vec3_t vec3_addv(vec3_t a, float b)
{
	return (vec3_t){a.x + b, a.y + b, a.z + b};
}

static inline vec3_t vec3_subv(vec3_t a, float b)
{
	return (vec3_t){a.x - b, a.y - b, a.z - b};
}

static inline vec3_t vec3_mulv(vec3_t a, float b)
{
	return (vec3_t){a.x * b, a.y * b, a.z * b};
}

static inline vec3_t vec3_divv(vec3_t a, float b)
{
	return (vec3_t){a.x / b, a.y / b, a.z / b};
}

static inline float vec3_lensq(vec3_t a)
{
	return fsq(a.x) + fsq(a.y) + +fsq(a.z);
}

static inline float vec3_len(vec3_t a)
{
	return sqrtf(vec3_lensq(a));
}

static inline float vec3_distsq(vec3_t a, vec3_t b)
{
	return fsq(a.x - b.x) + fsq(a.y - b.y) + fsq(a.z - b.z);
}

static inline float vec3_dist(vec3_t a, vec3_t b)
{
	return sqrtf(vec3_distsq(a, b));
}

static inline int vec3_is_equal(vec3_t a, vec3_t b)
{
	return fequal(a.x, b.x) && fequal(a.y, b.y) && fequal(a.z, b.z);
}

static inline vec3_t vec3_lerp(vec3_t a, vec3_t b, float t)
{
	return (vec3_t){flerp(a.x, b.x, t), flerp(a.y, b.y, t), flerp(a.z, b.z, t)};
}

static inline vec3_t vec3_min(vec3_t a, vec3_t b)
{
	return (vec3_t){fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.x, b.x)};
}

static inline vec3_t vec3_max(vec3_t a, vec3_t b)
{
	return (vec3_t){fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.x, b.x)};
}

static inline vec3_t vec3_normalize(vec3_t a)
{
	float l = vec3_len(a);
	if (l > 0)
		return (vec3_t){a.x / l, a.y / l, a.z / l};
	return vec3_zero();
}

static inline vec3_t vec3_invert(vec3_t a)
{
	return (vec3_t){1.0 / a.x, 1.0 / a.y, 1.0 / a.z};
}

static inline float vec3_dot(vec3_t a, vec3_t b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline vec3_t vec3_cross(vec3_t a, vec3_t b)
{
	return (vec3_t){
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x,
	};
}

static inline vec3_t vec3_transform(vec3_t v, mat4_t m)
{
	return (vec3_t){
		v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + 1.0 * m.m30,
		v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + 1.0 * m.m31,
		v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + 1.0 * m.m32,
	};
}

static inline float vec3_angle(vec3_t a, vec3_t b)
{

	return atan2f(vec3_len(vec3_cross(a, b)), vec3_dot(a, b));
}

static inline vec3_t vec3_rotate_axis(vec3_t v, vec3_t axis, float angle_rad)
{
	// Ref:
	// https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
	axis = vec3_normalize(axis);
	float c = cosf(angle_rad);
	float s = sinf(angle_rad);
	vec3_t p0 = vec3_mulv(v, c);
	vec3_t p1 = vec3_mulv(vec3_cross(axis, v), s);
	vec3_t p2 = vec3_mulv(axis, vec3_dot(axis, v) * (1 - c));
	return vec3_add(vec3_add(p0, p1), p2);
}

static inline vec3_t vec3_barycentric(vec3_t p, vec3_t a, vec3_t b, vec3_t c)
{
	// Ref:
	// https://realtimecollisiondetection.net/books/rtcd/
	vec3_t v0 = vec3_sub(b, a);
	vec3_t v1 = vec3_sub(c, a);
	vec3_t v2 = vec3_sub(p, a);
	float d00 = vec3_dot(v0, v0);
	float d01 = vec3_dot(v0, v1);
	float d11 = vec3_dot(v1, v1);
	float d20 = vec3_dot(v2, v0);
	float d21 = vec3_dot(v2, v1);
	float denom = d00 * d11 - d01 * d01;
	vec3_t result = vec3_zero();
	result.y = (d11 * d20 - d01 * d21) / denom; // v
	result.z = (d00 * d21 - d01 * d20) / denom; // w
	result.x = 1.0 - result.y - result.z;		// u
	return result;
}

// Reference: https://docs.gl/sl4/reflect
// TODO: confirm if math is right or not
static inline vec3_t vec3_reflect(vec3_t I, vec3_t N)
{
	// I - 2.0 * dot(N, I) * N
	return vec3_sub(I, vec3_mulv(N, vec3_dot(N, I) * 2.0));
}

// ---------------------------------------------------------------------------
// MATRIX4

// clang-format off
static inline mat4_t mat4_zero()
{
	return (mat4_t) {0};
}

static inline mat4_t mat4_identity()
{
	return (mat4_t) {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
}

static inline mat4_t mat4_mul(mat4_t a, mat4_t b) {
	return (mat4_t) {
		(a.m00 * b.m00) + (a.m01 * b.m10) + (a.m02 * b.m20) + (a.m03 * b.m30),
		(a.m00 * b.m01) + (a.m01 * b.m11) + (a.m02 * b.m21) + (a.m03 * b.m31),
		(a.m00 * b.m02) + (a.m01 * b.m12) + (a.m02 * b.m22) + (a.m03 * b.m32),
		(a.m00 * b.m03) + (a.m01 * b.m13) + (a.m02 * b.m23) + (a.m03 * b.m33),
		
		(a.m10 * b.m00) + (a.m11 * b.m10) + (a.m12 * b.m20) + (a.m13 * b.m30),
		(a.m10 * b.m01) + (a.m11 * b.m11) + (a.m12 * b.m21) + (a.m13 * b.m31),
		(a.m10 * b.m02) + (a.m11 * b.m12) + (a.m12 * b.m22) + (a.m13 * b.m32),
		(a.m10 * b.m03) + (a.m11 * b.m13) + (a.m12 * b.m23) + (a.m13 * b.m33),
		
		(a.m20 * b.m00) + (a.m21 * b.m10) + (a.m22 * b.m20) + (a.m23 * b.m30),
		(a.m20 * b.m01) + (a.m21 * b.m11) + (a.m22 * b.m21) + (a.m23 * b.m31),
		(a.m20 * b.m02) + (a.m21 * b.m12) + (a.m22 * b.m22) + (a.m23 * b.m32),
		(a.m20 * b.m03) + (a.m21 * b.m13) + (a.m22 * b.m23) + (a.m23 * b.m33),
		
		(a.m30 * b.m00) + (a.m31 * b.m10) + (a.m32 * b.m20) + (a.m33 * b.m30),
		(a.m30 * b.m01) + (a.m31 * b.m11) + (a.m32 * b.m21) + (a.m33 * b.m31),
		(a.m30 * b.m02) + (a.m31 * b.m12) + (a.m32 * b.m22) + (a.m33 * b.m32),
		(a.m30 * b.m03) + (a.m31 * b.m13) + (a.m32 * b.m23) + (a.m33 * b.m33),
	};
}

static inline mat4_t mat4_transpose(mat4_t m)
{
	return (mat4_t) {
		m.m00, m.m10, m.m20, m.m30,
		m.m01, m.m11, m.m21, m.m31,
		m.m02, m.m12, m.m22, m.m32,
		m.m03, m.m13, m.m23, m.m33,
	};
}

// Ref: https://journalppw.com/index.php/jpsp/article/download/1313/668/1516

static inline mat4_t mat4_translate(vec3_t t)
{
	return (mat4_t) {
		  1,   0,   0, 0,
		  0,   1,   0, 0,
		  0,   0,   1, 0,
		t.x, t.y, t.z, 1,
	};
}

static inline mat4_t mat4_rotate_x(float angle_rad)
{
	float c = cosf(angle_rad);
	float s = sinf(angle_rad);
	return (mat4_t) {
		 1,  0,  0,  0,
		 0,  c, -s,  0,
		 0,  s,  c,  0,
		 0,  0,  0,  1
	};
}

static inline mat4_t mat4_rotate_y(float angle_rad)
{
	float c = cosf(angle_rad);
	float s = sinf(angle_rad);
	return (mat4_t) {
		 c,  0,  s,  0,
		 0,  1,  0,  0,
		-s,  0,  c,  0,
		 0,  0,  0,  1,
	};
}

static inline mat4_t mat4_rotate_z(float angle_rad)
{
	float c = cosf(angle_rad);
	float s = sinf(angle_rad);
	return (mat4_t) {
		 c, -s,  0,  0,
		 s,  c,  0,  0,
		 0,  0,  1,  0,
		 0,  0,  0,  1,
	};
}

static inline mat4_t mat4_scale(vec3_t s)
{
	return (mat4_t) {
		 s.x,   0,   0,   0,
		   0, s.y,   0,   0,
		   0,   0, s.z,   0,
		   0,   0,   0,   1,
	};
}

// Wikipedia specifies axis aligned rotation for column major rotation
// As we are using row major we will use transpose of it
// Ref: https://en.wikipedia.org/wiki/Rotation_matrix#General_3D_rotations

static inline mat4_t mat4_rotate_axis(vec3_t axis, float angle_rad)
{
	float c = cosf(angle_rad);
	float s = sinf(angle_rad);
	vec3_t u = vec3_normalize(axis);
	return (mat4_t) {
		        fsq(u.x) * (1 - c) + c, u.x * u.y * (1 - c) + u.z * s, u.x * u.z * (1 - c) - u.y * s, 0,
		u.x * u.y * (1 - c) - u.z * s,         fsq(u.y) * (1 - c) + c, u.y * u.z * (1 - c) + u.x * s, 0,
		u.x * u.z * (1 - c) + u.y * s, u.y * u.z * (1 - c) - u.x * s,         fsq(u.z) * (1 - c) + c, 0,
		                            0,                             0,                             0, 1,
	};
}

// Ref: https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
// Ref: https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatrh
// Ref: https://stackoverflow.com/questions/349050/calculating-a-lookat-matrix

static inline mat4_t mat4_lookat_lh(vec3_t eye, vec3_t center, vec3_t up)
{
	vec3_t f = vec3_normalize(vec3_sub(center, eye)); // vz
	vec3_t s = vec3_normalize(vec3_cross(up, f)); // vx
	vec3_t u = vec3_cross(f, s); // vy
	
	return (mat4_t) {
		              s.x,               u.x,               f.x, 0,
		              s.y,               u.y,               f.y, 0,
		              s.z,               u.z,               f.z, 0,
		-vec3_dot(s, eye), -vec3_dot(u, eye), -vec3_dot(f, eye), 1,
	};
}

static inline mat4_t mat4_lookat_rh(vec3_t eye, vec3_t center, vec3_t up)
{
	vec3_t f = vec3_normalize(vec3_sub(eye, center)); // vz
	vec3_t s = vec3_normalize(vec3_cross(up, f)); // vx
	vec3_t u = vec3_cross(f, s); // vy
	
	return (mat4_t) {
		              s.x,               u.x,               f.x, 0,
		              s.y,               u.y,               f.y, 0,
		              s.z,               u.z,               f.z, 0,
		-vec3_dot(s, eye), -vec3_dot(u, eye), -vec3_dot(f, eye), 1,
	};
}


// Reference uses column major
// Ref: https://docs.gl/gl3/glFrustum

// static inline mat4_t mat4_frustum(float left, float right, float bottom, float top, float near, float far)
// {
// 
// }

// Reference uses column major
// Ref: https://docs.gl/gl3/glOrtho

static inline mat4_t mat4_ortho(float left, float right, float bottom, float top, float near, float far)
{
	vec3_t t = (vec3_t) {
		-(right + left) / (right - left),
		-(top + bottom) / (top - bottom),
		-(far + near)   / (far - near),
	};
	return (mat4_t) {
		2 / (right - left),                  0,                 0, 0,
		                 0, 2 / (top - bottom),                 0, 0,
		                 0,                  0, -2 / (far - near), 0,
		               t.x,                t.y,               t.z, 1,
	};	
}

static inline mat4_t mat4_perspective(float fov_rad, float aspect, float n, float f) {
// Ref: https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix.html
	float scale = tanf(fov_rad * 0.5) * n; 
    float t = scale;
    float b = -t;
    float r = t * aspect;
    float l  = -r;
	return (mat4_t) {
		  2 * n / (r - l),                 0,                     0,  0,
		                0,   2 * n / (t - b),                     0,  0,
		(r + l) / (r - l), (t + b) / (t - b),     -(f + n)/ (f - n), -1,
		                0,                 0, - 2 * f * n / (f - n),  0
	};
}

// TODO: confirm if the math is right or not
static inline mat4_t mat4_inverse(mat4_t m)
{
	// Matrix result = { 0 };
	
	    // Cache the matrix values (speed optimization)
	    float a00 = m.m00, a01 = m.m01, a02 = m.m02, a03 = m.m03;
	    float a10 = m.m10, a11 = m.m11, a12 = m.m12, a13 = m.m13;
	    float a20 = m.m20, a21 = m.m21, a22 = m.m22, a23 = m.m23;
	    float a30 = m.m30, a31 = m.m31, a32 = m.m32, a33 = m.m33;
	
	    float b00 = a00*a11 - a01*a10;
	    float b01 = a00*a12 - a02*a10;
	    float b02 = a00*a13 - a03*a10;
	    float b03 = a01*a12 - a02*a11;
	    float b04 = a01*a13 - a03*a11;
	    float b05 = a02*a13 - a03*a12;
	    float b06 = a20*a31 - a21*a30;
	    float b07 = a20*a32 - a22*a30;
	    float b08 = a20*a33 - a23*a30;
	    float b09 = a21*a32 - a22*a31;
	    float b10 = a21*a33 - a23*a31;
	    float b11 = a22*a33 - a23*a32;
	
	    // Calculate the invert determinant (inlined to avoid double-caching)
	    float invDet = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

		mat4_t result = {0};
	    result.m00 = (a11*b11 - a12*b10 + a13*b09)*invDet;
	    result.m01 = (-a01*b11 + a02*b10 - a03*b09)*invDet;
	    result.m02 = (a31*b05 - a32*b04 + a33*b03)*invDet;
	    result.m03 = (-a21*b05 + a22*b04 - a23*b03)*invDet;
	    result.m10 = (-a10*b11 + a12*b08 - a13*b07)*invDet;
	    result.m11 = (a00*b11 - a02*b08 + a03*b07)*invDet;
	    result.m12 = (-a30*b05 + a32*b02 - a33*b01)*invDet;
	    result.m13 = (a20*b05 - a22*b02 + a23*b01)*invDet;
	    result.m20 = (a10*b10 - a11*b08 + a13*b06)*invDet;
	    result.m21 = (-a00*b10 + a01*b08 - a03*b06)*invDet;
	    result.m22 = (a30*b04 - a31*b02 + a33*b00)*invDet;
	    result.m23 = (-a20*b04 + a21*b02 - a23*b00)*invDet;
	    result.m30 = (-a10*b09 + a11*b07 - a12*b06)*invDet;
	    result.m31 = (a00*b09 - a01*b07 + a02*b06)*invDet;
	    result.m32 = (-a30*b03 + a31*b01 - a32*b00)*invDet;
	    result.m33 = (a20*b03 - a21*b01 + a22*b00)*invDet;
	    return result;
}

// clang-format on

// TODO: mat4_determinant mat4_invert mat4_add mat4_sub mat4_mul mat4_frustum
// mat4_trace

#endif // IVY_MATH_H
