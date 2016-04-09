#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <math.h>
#include "vector.h"

class mat4 {
	public: 
		float m[4][4];

		mat4(void);
		mat4(const float in[4][4]);
		mat4::mat4(const quat4 in);

		mat4 operator*(const mat4 in);
		vec3 operator*(const vec3 in);

		void scale(const float s);
		void scale(const vec3 s);
};

#endif
