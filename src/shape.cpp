//  _____ _
// /  ___| |
// \ `--.| |__   __ _ _ __   ___
//  `--. \ '_ \ / _` | '_ \ / _ \
// /\__/ / | | | (_| | |_) |  __/
// \____/|_| |_|\__,_| .__/ \___|
//                   | |		 Classes
//                   |_|

#include "shape.h"

// -------------------------------------------------------------------------------------------------
// Plane Shape
// -------------------------------------------------------------------------------------------------
cPlane::cPlane(vec3 p, vec3 n, vec3 c) {
	color = c;
	normal = n;
	position = p;
	shininess = 0.15f;
	specPower = 16.0f;
}

bool cPlane::intersect(cRay *ray, hit_t &hit) {
	float denom = vec3::dot(normal, ray->direction);
	if (std::abs(denom) > 0.0001f) {
		float t = vec3::dot(position - ray->origin, normal) / denom;
		if (t >= 0.0f) {
			hit.point = ray->origin + (ray->direction * t);
			return true;
		}
	}
	return false;
}

vec3 cPlane::getColor(hit_t hit) {
	if (abs((int) (hit.point.x * 0.15f)) % 2 == abs((int) (hit.point.z * 0.15f)) % 2)
		return vec3(1.0f);
	return vec3(0.0f);
}

vec3 cPlane::getNormal(hit_t hit) {
	return normal;
}

// -------------------------------------------------------------------------------------------------
// Quad Shape
// -------------------------------------------------------------------------------------------------
cQuad::cQuad(vec3 p, vec3 v1, vec3 v2, vec3 v3, vec3 v4, vec3 c) {
	color = c;
	position = p;

	v[0] = v1;
	v[1] = v2;
	v[2] = v3;
	v[3] = v4;

	normal = vec3::cross(v[1] - v[0], v[2] - v[0]);
	normal.normalise();

	tu = v[1] - v[0];
	tv = v[2] - v[0];
	uu = vec3::dot(tu, tu);
	uv = vec3::dot(tu, tv);
	vv = vec3::dot(tv, tv);

	tu2 = v[0] - v[3];
	tv2 = v[2] - v[3];
	uu2 = vec3::dot(tu2, tu2);
	uv2 = vec3::dot(tu2, tv2);
	vv2 = vec3::dot(tv2, tv2);

	d = uv * uv - uu * vv;
	d2 = uv2 * uv2 - uu2 * vv2;

	shininess = 0.15f;
	specPower = 16.0f;
}

bool cQuad::intersect(cRay *ray, hit_t &hit) {
	float b = vec3::dot(normal, ray->direction);
	if (std::abs(b) < 0.00000001f)
		return false;

	float r = -vec3::dot(normal, ray->origin - (position + v[0])) / b;
	if (r < 0.0f)
		return false;

	vec3 point = ray->origin + (ray->direction * r);

	vec3 w = point - (position + v[0]);
	float wu = vec3::dot(w, tu);
	float wv = vec3::dot(w, tv);

	bool h = true;

	// 1st Triangle
	float s = 0.0f, t = 0.0f;
	s = (uv * wv - vv * wu) / d;
	if (s < 0.0f || s > 1.0f) {
		h = false;
	} else {
		t = (uv * wu - uu * wv) / d;
		if (t < 0.0f || (s + t) > 1.0f)
			h = false;
	}

	// Early out
	if (h) {
		hit.uv = vec2(s, t);
		hit.point = point;
		return true;
	}

	// 2nd Triangle
	w = point - (position + v[3]);
	wu = vec3::dot(w, tu2);
	wv = vec3::dot(w, tv2);
	s = (uv2 * wv - vv2 * wu) / d2;
	if (s < 0.0f || s > 1.0f) {
		return false;
	} else {
		t = (uv2 * wu - uu2 * wv) / d2;
		if (t < 0.0f || (s + t) > 1.0f)
			return false;
	}

	hit.uv = vec2(s, t);
	hit.point = point;
	return true;
}

vec3 cQuad::getColor(hit_t hit) {
	return vec3(hit.uv.s, 0.0f, 0.0f);

	//return color;
	//if (abs((int) (point.x * 0.15f)) % 2 == abs((int) (point.z * 0.15f)) % 2)
	//	return vec3(1.0f);
	//return vec3(0.0f);
}

vec3 cQuad::getNormal(hit_t hit) {
	return normal;
}

// -------------------------------------------------------------------------------------------------
// Sphere Shape
// -------------------------------------------------------------------------------------------------
cSphere::cSphere(vec3 p, float r, vec3 c) {
	color = c;
	position = p;
	radius = r;
	shininess = 0.75f;
	specPower = 128.0f;
}

bool cSphere::intersect(cRay *ray, hit_t &hit) {
	vec3 mid = ray->origin - position;
	float dist2 = vec3::dot(mid, mid);
	float r2 = (radius * radius);

	if(dist2 <= r2)
		return false;

	float a = vec3::dot(ray->direction, ray->direction);
	float b = 2.0f * vec3::dot(ray->direction, mid);
	float c = vec3::dot(mid, mid) - r2;
	float d = b * b - 4.0f * a * c;

	if (d < 0.0f)
		return false;

	float q;
	float d2 = sqrt(d);
	if (b < 0.0f)
		q = (-b - d2) / 2.0f;
	else
		q = (-b + d2) / 2.0f;

	float t0 = q / a;
	float t1 = c / q;
	if (t0 > t1) {
		float temp = t0;
		t0 = t1;
		t1 = temp;
	}

	if (t1 < 0.0f)
		return false;

	float t;
	if (t0 < 0.0f)
		t = t1;
	else
		t = t0;

	hit.point.x = ray->origin.x + t * ray->direction.x;
	hit.point.y = ray->origin.y + t * ray->direction.y;
	hit.point.z = ray->origin.z + t * ray->direction.z;
	return true;
}

vec3 cSphere::getColor(hit_t hit) {
	return color;
}

vec3 cSphere::getNormal(hit_t hit) {
	vec3 normal = hit.point - position;
	normal.normalise();
	return normal;
}
