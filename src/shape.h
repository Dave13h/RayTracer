#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "camera.h"
#include "ray.h"
#include "vector.h"

class cShape {
	public:
		vec3 color;
		vec3 position;
		float shininess;
		float specPower;

		virtual bool intersect(cRay *ray, hit_t &hit) { return false; };
		virtual vec3 getColor(hit_t hit) { return vec3(0.0f); }
		virtual vec3 getNormal(hit_t hit) { return vec3(0.0f); }
};

class cPlane : public cShape {
	public:
		vec3 normal;

		cPlane(vec3 p, vec3 n, vec3 c);

		bool intersect(cRay *ray, hit_t &hit);
		vec3 getColor(hit_t hit);
		vec3 getNormal(hit_t hit);
};

class cQuad : public cShape {
	public:
		vec3 normal; 	// Face Normal
		float dot; 		// Face Dot

		vec3 v[4];  	// Verts
		vec3 tu, tv, tu2, tv2;
		float uu, uv, vv, uu2, uv2, vv2, d, d2;

		cQuad(vec3 p, vec3 v1, vec3 v2, vec3 v3, vec3 v4, vec3 c);

		bool intersect(cRay *ray, hit_t &hit);
		vec3 getColor(hit_t hit);
		vec3 getNormal(hit_t hit);
};

class cSphere : public cShape {
	public:
		float radius;

		cSphere(vec3 p, float r, vec3 c);

		bool intersect(cRay *ray, hit_t &hit);
		vec3 getColor(hit_t hit);
		vec3 getNormal(hit_t hit);
};

#endif
