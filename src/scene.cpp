#include "scene.h"
#include "main.h"

cScene::cScene(void) {
	// ----- Basic Animated Scene -----
	shapes.push_back(new cPlane(vec3(0.0f, -20.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.5f)));

	shapes.push_back(new cSphere(vec3(  0.0f,   0.0f, -50.0f), 10.0f, vec3(1.0f, 0.0f, 0.0f)));
	shapes.push_back(new cSphere(vec3(-20.0f, -20.0f, -80.0f), 10.0f, vec3(0.0f, 1.0f, 0.0f)));
	shapes.push_back(new cSphere(vec3(  5.0f,   5.0f, -30.0f),  5.0f, vec3(0.0f, 0.0f, 1.0f)));

/*
	shapes.push_back(new cQuad(
		vec3(-10.0f, -10.0f, -10.0f), 	// Pos/Origin
		vec3(-10.0f,  20.0f,  -5.0f),	// v1
		vec3(-10.0f,   0.0f,  -5.0f),	// v2
		vec3( 10.0f,   0.0f,  -5.0f),	// v3
		vec3( 10.0f,  20.0f,  -5.0f),	// v4
		vec3(  0.5f,  0.25f,  0.5f)		// Colour
	));
*/

	lights.push_back(new cLight(vec3( 50.0f,  50.0f,  50.0f), vec3(0.5f,  0.5f,  0.5f), 250.0f));
	lights.push_back(new cLight(vec3(  0.0f, -10.0f, -30.0f), vec3(0.75f, 0.05f, 0.05f), 25.0f));
	lights.push_back(new cLight(vec3(-20.0f, -10.0f, -50.0f), vec3(0.05f, 0.75f, 0.05f), 25.0f));
	lights.push_back(new cLight(vec3( 20.0f, -10.0f, -50.0f), vec3(0.05f, 0.05f, 0.75f), 25.0f));

/*
	// ----- Axises Scene -----
	shapes.push_back(new cPlane(vec3(0.0f, -20.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.5)));

	shapes.push_back(new cSphere(vec3(  0.0f,   0.0f,   0.0f), 10.0f, vec3(1.0f, 1.0f, 1.0f)));

	shapes.push_back(new cSphere(vec3( 50.0f, 0.0f, 0.0f), 10.0f, vec3( 1.0f, 0.0f, 0.0f)));
	shapes.push_back(new cSphere(vec3(100.0f, 0.0f, 0.0f), 10.0f, vec3( 1.0f, 0.0f, 0.0f)));
	shapes.push_back(new cSphere(vec3(150.0f, 0.0f, 0.0f), 10.0f, vec3( 1.0f, 0.0f, 0.0f)));

	shapes.push_back(new cSphere(vec3(0.0f,  50.0f, 0.0f), 10.0f, vec3(0.0f, 1.0f, 0.0f)));
	shapes.push_back(new cSphere(vec3(0.0f, 100.0f, 0.0f), 10.0f, vec3(0.0f, 1.0f, 0.0f)));
	shapes.push_back(new cSphere(vec3(0.0f, 150.0f, 0.0f), 10.0f, vec3(0.0f, 1.0f, 0.0f)));

	shapes.push_back(new cSphere(vec3(0.0f, 0.0f,  -50.0f), 10.0f, vec3(0.0f, 0.0f, 1.0f)));
	shapes.push_back(new cSphere(vec3(0.0f, 0.0f, -100.0f), 10.0f, vec3(0.0f, 0.0f, 1.0f)));
	shapes.push_back(new cSphere(vec3(0.0f, 0.0f, -150.0f), 10.0f, vec3(0.0f, 0.0f, 1.0f)));

	lights.push_back(new cLight(vec3(50.0f, 50.0f, 50.0f), vec3(0.5f, 0.5f, 0.5f), 250.0f));
	lights.push_back(new cLight(vec3(-20.0f,   0.0f, -20.0f), vec3(0.75f,  0.0f, 0.0f), 150.0f));
*/

	shapeCount = (int) shapes.size();
	lightCount = (int) lights.size();
}

void cScene::update(const double dt) {
	static float ticker = 0.0f;
	ticker += 90.0f * (float) dt;
	if (ticker > 360.0f)
		ticker -= 360.0f;

	shapes[1]->position.z = -50.0f + (cos(DEG2RAD(ticker)) * 10.0f);
	shapes[2]->position.y = -20.0f + (cos(DEG2RAD(ticker)) *  4.0f);
	shapes[3]->position.x =   5.0f + (sin(DEG2RAD(ticker)) *  5.0f);
	shapes[3]->position.y =   5.0f + (sin(DEG2RAD(ticker)) *  3.0f);

//	shapes[4]->position.x =   5.0f + (sin(DEG2RAD(ticker)) *  3.0f);
//	shapes[4]->position.y =   5.0f + (cos(DEG2RAD(ticker)) *  3.0f);
}
