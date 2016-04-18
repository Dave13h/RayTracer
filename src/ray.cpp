// ______
// | ___ \
// | |_/ /__ _ _   _
// |    // _` | | | |
// | |\ \ (_| | |_| |
// \_| \_\__,_|\__, |
//              __/ |
//             |___/  Class

#include "ray.h"

cRay::cRay(vec3 p, vec3 d) {
	origin = p;
	direction = d;
}
