#ifndef __SCENE_H__
#define __SCENE_H__

#include "shape.h"
#include "light.h"

class cScene {
	public:
		std::vector<cShape*> shapes;
		std::vector<cLight*> lights;

		int shapeCount;
		int lightCount;

		cScene(void);

		void update(const double dt);
};

#endif
