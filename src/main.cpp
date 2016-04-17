#include "main.h"
#include "draw.h"
#include "jobmanager.h"
#include "light.h"
#include "ray.h"
#include "scene.h"
#include "shape.h"
#include "timer.h"

// Threading shenanigans
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

SDL_Event event;
extern SDL_Renderer *renderer;
SDL_Window *window;

unsigned int win_width = 640;
unsigned int win_height = 320;

extern bool r_interlaced;
extern unsigned int r_scale;
extern int r_height;
extern int r_width;
extern bool r_fullscreen;
extern double r_aspect;

extern unsigned int r_fps;
extern unsigned int r_frameCount;

extern bool r_pass;
extern bool r_paused;
extern bool r_running;

extern bool r_threadInfo;

extern bool r_reflections;
extern bool r_shadows;
extern bool r_vignette;

extern unsigned int r_bounces;
extern unsigned int r_hitCount;

extern unsigned int r_regions;

extern std::vector<region_t> regions;
extern std::vector<thread_t> threads;

std::atomic<int> threadCount;
std::mutex frameMutex;
std::condition_variable frameCond;

uint64_t waitTime = 0;
double waitTimed = 0.0;
uint64_t jobSubTime = 0;
double jobSubTimed = 0.0;

Uint64 cpuFreq;

cScene *scene;
cCamera *camera;

enum mousebutton_t {
	LEFT = 1,
	MIDDLE,
	RIGHT
};

struct mouse_t {
	bool captured;
	bool inverted;
	double sensitivity;
	int x;
	int y;
	std::map<mousebutton_t, bool> button;
} iMouse;
std::map<SDL_Keycode, bool> keyState;

// ----------------------------
// Input Functions
// ----------------------------
void inputProcess(const cTimer *timer, const SDL_Event event) {
	switch (event.type) {
		// SDL Core/Window Events -------
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					r_running = false;
					return;
					break;
			}
			break;

		case SDL_QUIT:
			r_running = false;
			return;
			break;

		// Mouse States -----------------
		case SDL_MOUSEBUTTONDOWN:
			iMouse.button[(mousebutton_t) event.button.button] = true;
			break;

		case SDL_MOUSEBUTTONUP:
			iMouse.button[(mousebutton_t) event.button.button] = false;
			break;

		case SDL_MOUSEMOTION:
			iMouse.x = -event.motion.xrel;
			iMouse.y = -event.motion.yrel;
			break;

		// Key states -------------------
		case SDL_KEYDOWN:
			keyState[event.key.keysym.sym] = true;
			break;

		case SDL_KEYUP:
			keyState[event.key.keysym.sym] = false;
			break;

		default:
			break;
	}
}

void inputCheck(const cTimer *timer) {

	// Home time
	if (keyState[SDLK_LALT] && keyState[SDLK_F4]) {
		r_running = false;
		keyState[SDLK_LALT] = false;
		keyState[SDLK_F4] = false;
	} else if (keyState[SDLK_ESCAPE]) {
		r_running = false;
		keyState[SDLK_ESCAPE] = false;
	}

	// Renderer Settings
	if (keyState[SDLK_i]) {
		r_interlaced = !r_interlaced;
		r_pass = true;
		keyState[SDLK_i] = false;
	}

	if (keyState[SDLK_t]) {
		r_threadInfo = !r_threadInfo;
		r_pass = true;
		keyState[SDLK_t] = false;
	}

	if (keyState[SDLK_r]) {
		r_reflections = !r_reflections;
		for (auto &thread : threads)
			thread.reflections = r_reflections;
		keyState[SDLK_r] = false;
	}

	if (keyState[SDLK_o]) {
		r_shadows = !r_shadows;
		for (auto &thread : threads)
			thread.shadows = r_shadows;
		keyState[SDLK_o] = false;
	}

	if (keyState[SDLK_v]) {
		r_vignette = !r_vignette;
		keyState[SDLK_v] = false;
	}

	// Camera Controls
	if (keyState[SDLK_a]) 		camera->velocity.x -= 10.0f * timer->dt;
	if (keyState[SDLK_d]) 		camera->velocity.x += 10.0f * timer->dt;
	if (keyState[SDLK_e]) 		camera->velocity.y -= 10.0f * timer->dt;
	if (keyState[SDLK_q]) 		camera->velocity.y += 10.0f * timer->dt;
	if (keyState[SDLK_w]) 		camera->velocity.z -= 10.0f * timer->dt;
	if (keyState[SDLK_s]) 		camera->velocity.z += 10.0f * timer->dt;

	if (keyState[SDLK_DOWN])	camera->angle.x -= 110.0f * timer->dt;
	if (keyState[SDLK_UP])		camera->angle.x += 110.0f * timer->dt;
	if (keyState[SDLK_LEFT])	camera->angle.y -= 110.0f * timer->dt;
	if (keyState[SDLK_RIGHT])	camera->angle.y += 110.0f * timer->dt;

	// Scaling
	if (keyState[SDLK_1]) {
		setScale(1);
		keyState[SDLK_1] = false;
	} else if (keyState[SDLK_2]) {
		setScale(2);
		keyState[SDLK_2] = false;
	} else if (keyState[SDLK_3]) {
		setScale(4);
		keyState[SDLK_3] = false;
	} else if (keyState[SDLK_4]) {
		setScale(8);
		keyState[SDLK_4] = false;
	}

	// Frame Control
	if (keyState[SDLK_SPACE]) {
		r_pass = true;
		keyState[SDLK_SPACE] = false;
	}
	if (keyState[SDLK_PAUSE]) {
		r_paused = !r_paused;
		drawFrameInfo(timer->now); // Update title ;)
		keyState[SDLK_PAUSE] = false;
	}
}

// ----------------------------
// Main loop
// ----------------------------
int main(int argc, char *argv[]) {
	threadCount = 0;

	SDL_Init(SDL_INIT_EVERYTHING);
	r_regions = std::thread::hardware_concurrency();

	// Parse switches ---------
	for (int a = 0; a < argc; ++a) {
		std::string arg(argv[a]);

		if (arg[0] != '-')
			continue;

		switch (arg[1]) {
			case 'b': {
				std::string bounces = arg.substr(2);
				int b = atoi(bounces.c_str());
				if (b < 1 || b > 32) {
					printf("Invalid input for max bounces => %s\n", bounces.c_str());
					break;
				}

				r_bounces = b;
				printf("Setting Bounces => %d\n", b);
				break;
			}

			case 'i':
				printf("Enabling Interlaced Mode\n");
				r_interlaced = true;
				break;

			case 'f':
				r_fullscreen = true;
				break;

			case 'h': {
				std::string height = arg.substr(2);
				int h = atoi(height.c_str());
				if (h < 16) {
					printf("Invalid input for window height => %s\n", height.c_str());
					break;
				}

				win_height = h;
				printf("Setting Window Height => %d\n", h);
				break;
			}

			case 'r':
				printf("Enabling Realtime Mode\n");
				r_paused = false;
				break;

			case 't': {
				std::string tCount = arg.substr(2);
				int ts = atoi(tCount.c_str());
				if (ts < 0 || ts > 32) {
					printf("Invalid input for Thread Count => %s\n", tCount.c_str());
					break;
				}

				r_regions = ts;
				printf("Setting Thread Count => %d\n", ts);
				break;
			}

			case 'w': {
				std::string width = arg.substr(2);
				int w = atoi(width.c_str());
				if (w < 16) {
					printf("Invalid input for window width => %s\n", width.c_str());
					break;
				}

				win_width = w;
				printf("Setting Window Width => %d\n", w);
				break;
			}

			default:
				printf("Unknown Argument[%d] => %s\n", a, arg.c_str());
				break;
		}
	}


	std::string cpuFeats;
	if (SDL_HasRDTSC())		cpuFeats += "RDTSC ";
	if (SDL_HasAltiVec())	cpuFeats += "AltiVec ";
	if (SDL_HasMMX())		cpuFeats += "MMX ";
	if (SDL_Has3DNow())		cpuFeats += "3DNow! ";
	if (SDL_HasSSE())		cpuFeats += "SSE ";
	if (SDL_HasSSE2())		cpuFeats += "SSE2 ";
	if (SDL_HasSSE3())		cpuFeats += "SSE3 ";
	if (SDL_HasSSE41())		cpuFeats += "SSE4.1 ";
	if (SDL_HasSSE42())		cpuFeats += "SSE4.2 ";
	if (SDL_HasAVX())		cpuFeats += "AVX ";

	if (r_fullscreen) {
		SDL_DisplayMode mode;
		SDL_GetCurrentDisplayMode(0, &mode);
		printf("%d x %d\n", mode.w, mode.h);

		win_height = mode.h;
		win_width = mode.w;
	}

	printf("\n---------------------- Raytracer o' Doom ----------------------\n");
	printf("Window Size: %d x %d\n", win_width, win_height);
	printf("CPU Thread Count: %d\n", std::thread::hardware_concurrency());
	printf("CPU Cache Line Size: %dKB\n", SDL_GetCPUCacheLineSize());
	printf("CPU Feats: %s\n", cpuFeats.c_str());
	printf("Memory Size: %dMB\n", SDL_GetSystemRAM());
	printf("----------------------- Launch Options ------------------------\n");
	printf("-b<Number of bounces>       - Max number of bounces\n");
	printf("-h<height>                  - Window Height\n");
	printf("-i                          - Interlace Mode\n");
	printf("-r                          - Realtime Mode\n");
	printf("-t<Number of threads>       - Number of threads to spawn\n");
	printf("-w<width>                   - Window Width\n");
	printf("---------------------------- Keys -----------------------------\n");
	printf("Set Renderer Scale: [1 - 4]\n");
	printf("Set Interlaced Rendering: [i]\n");
	printf("Toggle Reflections: [r]\n");
	printf("Toggle Shadows: [o]\n");
	printf("Toggle ThreadInfo: [t]\n");
	printf("Toggle Vignette: [v]\n");
	printf("Enable Realtime Rendering: [PAUSE]\n");
	printf("---------------------------------------------------------------\n");

	// Setup the window -------
	window = SDL_CreateWindow("RayTracer!",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		win_width, win_height,
		(r_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));

	// Normal renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	SDL_RenderSetScale(renderer, (float) r_scale, (float) r_scale);

	drawInit();

	scene = new cScene();
	camera = new cCamera();
	cTimer *timer = new cTimer(cTimer::RUNNING);
	uint64_t timeLast = SDL_GetPerformanceCounter();

	// Spawn some workers
	jobManager jMan(std::thread::hardware_concurrency());

	std::unique_lock<std::mutex> frameLock(jMan.jMutex);
	frameLock.unlock();

	int r = 0;
	threads.resize(r_regions);
	for (auto &thread : threads) {
		thread.tid = r;
		thread.camera = camera;
		thread.region = &regions[r++];
		thread.retVal = -1;

		thread.stats.tTime = 0;
		thread.hitCount = 0;

		thread.reflections = r_reflections;
		thread.shadows = r_shadows;

		thread.cameraRay  = new cRay(camera->position, camera->direction);
		thread.reflectRay = new cRay(camera->position, camera->direction);
		thread.shadowRay  = new cRay(camera->position, camera->direction);
	}

	r_running = true;
	while (r_running) {
		cpuFreq = SDL_GetPerformanceFrequency();
		timer->tick();

		// Are we realtime? ---
		if (!r_paused) {
			r_pass = true;
		} else {
			// If not running realtime
			// wait for something to happen
			SDL_WaitEvent(&event);
			inputProcess(timer, event);
			inputCheck(timer);
		}

		// Event Handler ------
		while (SDL_PollEvent(&event))
			inputProcess(timer, event);

		// Rendering ----------
		if (r_pass) {
			inputCheck(timer);

			camera->update(timer->dt);
			scene->update(timer->dt);

			// Time to go to work!
			threadCount = r_regions;
			jobSubTime = SDL_GetPerformanceCounter();
			for (unsigned int t = 0; t < r_regions; ++t)
				jMan.addJob(drawStuffJob, &threads[t]);
			jobSubTimed = (double) (SDL_GetPerformanceCounter() - jobSubTime) /	cpuFreq;
			jMan.run();

			// Wait until they finish
			waitTime = SDL_GetPerformanceCounter();
			frameLock.lock();

			if (!jMan.jCondition.wait_for(frameLock, std::chrono::milliseconds(1000),
				[&jMan]{ return (jMan.jobCount == 0); })) {
				//printf("%d Thread(s) stalled!\n", jMan.jobCount);
			}

			frameLock.unlock();
			waitTimed = (double) (SDL_GetPerformanceCounter() - waitTime) /	cpuFreq;

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);

			blitRegions();
			postProcessing();
			outputFrame();

			r_hitCount = 0;
			for (unsigned int t = 0; t < r_regions; ++t)
				r_hitCount += threads[t].hitCount;

			drawFrameInfo(timer->now);
			SDL_RenderPresent(renderer);

			// Frame Stats
			++r_frameCount;
			if (timer->now - timeLast >= 1.0 * (float) cpuFreq) {
				r_fps = r_frameCount;
				r_frameCount = 0;
				timeLast = timer->now;
			}

			r_pass = false;
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
