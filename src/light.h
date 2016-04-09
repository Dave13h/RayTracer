#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "vector.h"

class cLight {
	public:
		vec3 position;
		vec3 color;
		float size;

		cLight::cLight(vec3 p, vec3 c, float s);
};

#endif
