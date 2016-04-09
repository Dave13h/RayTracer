#include "camera.h"

// ----------------------------
// Constructs
// ----------------------------
cCamera::cCamera(void) {
	angle 		= vec3(180.0f, 0.0f, 0.0f);
	position 	= vec3(0.0f);
	velocity 	= vec3(0.0f);

	direction 	= vec3(0.0f, 0.0f, -1.0f);
	direction.normalise();
	
	mat = mat4();

	fov = 45.0f;
	viewAngle = tanf(fov * DEG2RAD(0.5f));

	near = 0.1f;
	far = 100.0f;
}

void cCamera::update(const double dt) {
	if (angle.x > 360.0f)	angle.x -= 360.0f;
	else if (angle.x < 0.0f)angle.x += 360.0f;
	if (angle.y > 360.0f)	angle.y -= 360.0f;
	else if (angle.y < 0.0f)angle.y += 360.0f;

	rot = 
		quat4(DEG2RAD(angle.x), vec3(0.0f, 0.0f, 1.0f)) * 
		quat4(DEG2RAD(angle.y), vec3(0.0f, 1.0f, 0.0f));

	mat = mat4(rot);

	position += velocity * rot;
	velocity *= vec3(0.9f);
}
