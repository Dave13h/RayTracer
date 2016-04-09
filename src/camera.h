#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "main.h"
#include "matrix.h"	
#include "vector.h"

class cCamera {
	public:
		vec3 angle, direction, position, velocity;

		quat4 rot;
		mat4 mat;

		float fov, viewAngle;
		float near, far;

		cCamera(void);
		void update(const double dt);
};

#endif
