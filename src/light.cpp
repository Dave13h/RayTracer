//  _     _       _     _
// | |   (_)     | |   | |
// | |    _  __ _| |__ | |_
// | |   | |/ _` | '_ \| __|
// | |___| | (_| | | | | |_
// \_____/_|\__, |_| |_|\__|
//           __/ |          Class
//          |___/

#include "light.h"

cLight::cLight(vec3 p, vec3 c, float s) {
	color = c;
	position = p;
	size = s;
}
