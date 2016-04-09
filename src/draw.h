#ifndef __DRAW_H__
#define __DRAW_H__

	#include "main.h"
	#include "camera.h"
	#include "ray.h"
	#include "shape.h"
	#include "vector.h"

	// Threading shenanigans
	#include <condition_variable>
	#include <mutex>
	#include <thread>

	struct region_t {
		ivec2 width, height;
		Uint32 *pixels;
	};

	struct stats_t {
		double tTime;
	};

	struct thread_t {
		int tid;
		std::thread *handle;
		cCamera *camera;
		region_t *region;
		int retVal;

		cRay *cameraRay, *reflectRay, *shadowRay ;

		struct stats_t stats;

		int hitCount;

		bool reflections;
		bool shadows;

		bool condition;
		std::condition_variable *cond;
	};

	void drawInit(void);
	void drawFrameInfo(const uint64_t frameTime);
	int  drawStuff(void *data);
	void drawStuffJob(thread_t *tData);
	void setScale(const unsigned int scale);

	void resizeRegions(void);
	void blitRegions(void);
	void postProcessing(void);
	void outputFrame(void);

	bool inShadow(cRay* ray, cShape *curShape, const float lDist);
#endif
