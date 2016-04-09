#ifndef __RAY_H__
#define __RAY_H__

#include "vector.h"	

class cRay {
	public:
		vec3 direction;
		vec3 origin;

		cRay(vec3 p, vec3 d);
};

#endif
