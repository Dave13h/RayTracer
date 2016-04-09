#include "light.h"

cLight::cLight(vec3 p, vec3 c, float s) {
	color = c;
	position = p;
	size = s;
}
