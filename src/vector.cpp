//  _   _           _
// | | | |         | |
// | | | | ___  ___| |_ ___  _ __
// | | | |/ _ \/ __| __/ _ \| '__|
// \ \_/ /  __/ (__| || (_) | |
//  \___/ \___|\___|\__\___/|_|
// 								 Class

#include "vector.h"

#include <algorithm>
#include <smmintrin.h>

// -------------------------------------------------------------------------------------------------
// float Quat 4
// -------------------------------------------------------------------------------------------------
quat4::quat4(void) {
	x = 0.0;
	y = 0.0;
	z = 0.0;
	w = 0.0;
}

quat4::quat4(float ix, float iy, float iz, float iw) {
	x = ix;
	y = iy;
	z = iz;
	w = iw;
}

quat4::quat4(float angle, vec3 axis) {
	float s = sin(angle * 0.5f);
	x = axis.x * s;
	y = axis.y * s;
	z = axis.z * s;
	w = cos(angle * 0.5f);
}

// Operators
quat4 quat4::operator*(quat4 in) {
	return quat4(
		w * in.w - x * in.x - y * in.y - z * in.z,
		w * in.x + x * in.w + y * in.z - z * in.y,
		w * in.y + y * in.w + z * in.x - x * in.z,
		w * in.z + z * in.w + x * in.y - y * in.x
	);
}

// -------------------------------------------------------------------------------------------------
// float Vector 2
// -------------------------------------------------------------------------------------------------
vec2::vec2(void) {
	x = 0.0f;
	y = 0.0f;
}
vec2::vec2(float ix, float iy) {
	x = ix;
	y = iy;
}
vec2::vec2(float i) {
	x = i;
	y = i;
}

// Operators
vec2 vec2::operator+(vec2 in) {
	return vec2(x + in.x, y + in.y);
}
void vec2::operator+=(vec2 in) {
	x += in.x;	y += in.y;
}

vec2 vec2::operator-(vec2 in) {
	return vec2(x - in.x, y - in.y);
}
void vec2::operator-=(vec2 in) {
	x -= in.x; y -= in.y;
}
vec2 vec2::operator-(void) {
	return vec2(-x, -y);
}

vec2 vec2::operator*(vec2 in) {
	return vec2(x * in.x, y * in.y);
}
void vec2::operator*=(vec2 in) {
	x *= in.x; y *= in.y;
}

vec2 vec2::operator/(vec2 in) {
	return vec2(x / in.x, y / in.y);
}
void vec2::operator/=(vec2 in) {
	x /= in.x; y /= in.y;
}

bool vec2::operator==(vec2 in) {
	return (x == in.x && y == in.y);
}
bool vec2::operator!=(vec2 in) {
	return !(x == in.x && y == in.y);
}

// -------------------------------------------------------------------------------------------------
// float Vector 3
// -------------------------------------------------------------------------------------------------
vec3::vec3(void) {
	x = 0.0;
	y = 0.0;
	z = 0.0;
}
vec3::vec3(float ix, float iy, float iz) {
	x = ix;
	y = iy;
	z = iz;
}
vec3::vec3(float i) {
	x = i;
	y = i;
	z = i;
}
vec3::vec3(ivec3 in) {
	x = (float) in.r / 255;
	y = (float) in.g / 255;
	z = (float) in.b / 255;
}

// Operators
vec3 vec3::operator+(vec3 in) {
	return vec3(x + in.x, y + in.y, z + in.z);
}
void vec3::operator+=(vec3 in) {
	x += in.x; y += in.y; z += in.z;
}

vec3 vec3::operator-(vec3 in) {
	return vec3(x - in.x, y - in.y, z - in.z);
}
vec3 vec3::operator-(float in) {
	return vec3(x - in, y - in, z - in);
}
void vec3::operator-=(vec3 in) {
	x -= in.x; y -= in.y; z -= in.z;
}
vec3 vec3::operator-(void) {
	return vec3(-x, -y, -z);
}

vec3 vec3::operator*(vec3 in) {
	return vec3(x * in.x, y * in.y, z * in.z);
}
vec3 vec3::operator*(float in) {
	return vec3(x * in, y * in, z * in);
}
vec3 vec3::operator*(quat4 in) {
	vec3 t = vec3::cross(in, *this) * 2.0f;
	return *this + (t * in.w) + vec3::cross(in, t);;
}
void vec3::operator*=(vec3 in) {
	x *= in.x; y *= in.y; z *= in.z;
}

vec3 vec3::operator/(vec3 in) {
	return vec3(x / in.x, y / in.y, z / in.z);
}
void vec3::operator/=(vec3 in) {
	x /= in.x; y /= in.y; z /= in.z;
}
void vec3::operator/=(float in) {
	x /= in; y /= in; z /= in;
}

bool vec3::operator==(vec3 in) {
	return (x == in.x && y == in.y && z == in.z);
}
bool vec3::operator!=(vec3 in) {
	return !(x == in.x && y == in.y && z == in.z);
}

// Methods
vec3 vec3::cross(vec3 in) {
	return vec3(y * in.z - z * in.y,
				z * in.x - x * in.z,
				x * in.y - y * in.x);
}

vec3 vec3::cross(vec3 v1, vec3 v2) {
	return vec3(v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x);
}
vec3 vec3::cross(quat4 v1, vec3 v2) {
	return vec3(v1.y * v2.z - v1.z * v2.y,
				v1.z * v2.x - v1.x * v2.z,
				v1.x * v2.y - v1.y * v2.x);
}

float vec3::dot(vec3 v1, vec3 v2) {
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
/*
	// Slow SSE DotProduct =(
	float result;
	__m128 a = _mm_load_ps( v1.v );
	__m128 b = _mm_load_ps( v2.v );
	_mm_store_ss(&result, _mm_dp_ps(a, b, 0x7F));
    return result;
*/
}
float vec3::dot(vec3 in) {
	return (x * in.x + y * in.y + z * in.z);
}

float vec3::length(void) {
	return (float) sqrt(x * x + y * y + z * z);
}
float vec3::length(vec3 in) {
	return (float) sqrt(in.x * in.x + in.y * in.y + in.z * in.z);
}
vec3 vec3::max(vec3 in, float m) {
	return vec3(std::max(in.x, m), std::max(in.y, m), std::max(in.z, m));
}
vec3 vec3::min(vec3 in, float m) {
	return vec3(std::min(in.x, m), std::min(in.y, m), std::min(in.z, m));
}
void vec3::normalise(void) {
	float mag = (1.0f / sqrt(x * x + y * y + z * z));
	x *= mag;
	y *= mag;
	z *= mag;
}
vec3 vec3::pow(vec3 in, float p) {
	return vec3(std::pow(in.x, p), std::pow(in.y, p), std::pow(in.z, p));
}
vec3 vec3::reflect(vec3 in, vec3 normal) {
	return vec3((in - 2.0f * vec3::dot(in, normal)) * normal);
}

// -------------------------------------------------------------------------------------------------
// integer Vector 3
// -------------------------------------------------------------------------------------------------
ivec3::ivec3(void) {
	x = 0;
	y = 0;
	z = 0;
}
ivec3::ivec3(int ix, int iy, int iz) {
	x = ix;
	y = iy;
	z = iz;
}
ivec3::ivec3(int i) {
	x = i;
	y = i;
	z = i;
}

// Operators
ivec3 ivec3::operator+(ivec3 in) {
	return ivec3(x + in.x, y + in.y, z + in.z);
}
void ivec3::operator+=(ivec3 in) {
	x += in.x;	y += in.y;	z += in.z;
}

ivec3 ivec3::operator-(ivec3 in) {
	return ivec3(x - in.x, y - in.y, z - in.z);
}
void ivec3::operator-=(ivec3 in) {
	x -= in.x; y -= in.y; z -= in.z;
}
ivec3 ivec3::operator-(void) {
	return ivec3(-x, -y, -z);
}

ivec3 ivec3::operator*(ivec3 in) {
	return ivec3(x * in.x, y * in.y, z * in.z);
}
ivec3 ivec3::operator*(float in) {
	int dx = (int) ((float) x * in);
	int dy = (int) ((float) y * in);
	int dz = (int) ((float) z * in);
	return ivec3(dx, dy, dz);
}
void ivec3::operator*=(ivec3 in) {
	x *= in.x; y *= in.y; z *= in.z;
}

ivec3 ivec3::operator/(ivec3 in) {
	return ivec3(x / in.x, y / in.y, z / in.z);
}
void ivec3::operator/=(ivec3 in) {
	x /= in.x; y /= in.y; z /= in.z;
}

bool ivec3::operator==(ivec3 in) {
	return (x == in.x && y == in.y && z == in.z);
}
bool ivec3::operator!=(ivec3 in) {
	return !(x == in.x && y == in.y && z == in.z);
}

// -------------------------------------------------------------------------------------------------
// integer Vector 2
// -------------------------------------------------------------------------------------------------
ivec2::ivec2(void) {
	x = 0;
	y = 0;
}
ivec2::ivec2(int ix, int iy) {
	x = ix;
	y = iy;
}
ivec2::ivec2(int i) {
	x = i;
	y = i;
}

// Operators
ivec2 ivec2::operator+(ivec2 in) {
	return ivec2(x + in.x, y + in.y);
}
void ivec2::operator+=(ivec2 in) {
	x += in.x;	y += in.y;
}

ivec2 ivec2::operator-(ivec2 in) {
	return ivec2(x - in.x, y - in.y);
}
void ivec2::operator-=(ivec2 in) {
	x -= in.x; y -= in.y;
}
ivec2 ivec2::operator-(void) {
	return ivec2(-x, -y);
}

ivec2 ivec2::operator*(ivec2 in) {
	return ivec2(x * in.x, y * in.y);
}
void ivec2::operator*=(ivec2 in) {
	x *= in.x; y *= in.y;
}

ivec2 ivec2::operator/(ivec2 in) {
	return ivec2(x / in.x, y / in.y);
}
void ivec2::operator/=(ivec2 in) {
	x /= in.x; y /= in.y;
}

bool ivec2::operator==(ivec2 in) {
	return (x == in.x && y == in.y);
}
bool ivec2::operator!=(ivec2 in) {
	return !(x == in.x && y == in.y);
}
