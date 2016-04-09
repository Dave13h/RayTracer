#include "main.h"
#include "camera.h"
#include "draw.h"
#include "scene.h"
#include "shape.h"
#include "ray.h"

#include <atomic>

SDL_Renderer *renderer;
SDL_Texture *blitTexture;
extern SDL_Window *window;

TTF_Font* r_font;
unsigned int r_fontSize = 10;
SDL_Color r_fontColor = {255, 255, 255};
SDL_Surface* r_fontSurface;
SDL_Texture* r_fontTexture;

extern unsigned int win_height;
extern unsigned int win_width;

bool r_interlaced = false;
unsigned int r_scale = 1;
int r_height;
int r_width;
bool r_fullscreen = false;
float r_aspect;

unsigned int r_fps = 0;
unsigned int r_frameCount = 1;

unsigned int r_bounces = 1;

bool r_pass = true;
bool r_paused = true;
bool r_running = true;

bool r_threadInfo = false;

bool r_reflections = true;
bool r_shadows = true;
bool r_vignette = true;

unsigned int r_hitCount = 0;

unsigned int r_regions = 1;

std::vector<region_t> regions;
std::vector<thread_t> threads;

extern std::atomic<int> threadCount;
extern std::mutex frameMutex;
extern std::condition_variable frameCond;

Uint32 *frameBuffer;
uint64_t blitTime = 0;
double blitTimed = 0.0;
extern double waitTimed;
extern double jobSubTimed;

uint64_t frameoutTime = 0;
double frameoutTimed = 0.0;

extern uint64_t cpuFreq;

extern std::vector<thread_t> threads;

extern cScene *scene;
extern cCamera *camera;

vec3 ambientColor(0.05f, 0.05f, 0.075f);

// ----------------------------
// Draw Functions
// ----------------------------
void drawInit(void) {
	r_height = win_height / r_scale;
	r_width = win_width / r_scale;
	r_aspect = (float) r_width / (float) r_height;

	// Font funs --------------
	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", TTF_GetError());
		exit(2);
	}
	r_font = TTF_OpenFont("fonts/CarroisGothic-Regular.ttf", r_fontSize);
	if (!r_font)
		printf("TTF_OpenFont: %s\n", TTF_GetError());

	// Regions ----------------
	regions.resize(r_regions);
	resizeRegions();
}

static int drawText(const int x, const int y, const std::string text) {
	SDL_Rect Message_rect;

	r_fontSurface = TTF_RenderText_Solid(r_font, text.c_str(), r_fontColor);
	r_fontTexture = SDL_CreateTextureFromSurface(renderer, r_fontSurface);

	Message_rect.x = x;
	Message_rect.y = y;
	Message_rect.w = r_fontSurface->w;
	Message_rect.h = r_fontSurface->h;

	SDL_RenderCopy(renderer, r_fontTexture, NULL, &Message_rect);
	SDL_FreeSurface(r_fontSurface);
	SDL_DestroyTexture(r_fontTexture);

	// Return the row height
	return r_fontSurface->h;
}

void drawFrameInfo(const uint64_t frameTime) {
	std::string tText;
	int lHeight = 0;
	tText = "[FPS: " + std::to_string(r_fps) + " / " +
		std::to_string((double) (SDL_GetPerformanceCounter() - frameTime) /	cpuFreq) + "s]" +
		"[Wait: " + std::to_string(waitTimed) + "s]" +
		"[QSub: " + std::to_string(jobSubTimed) + "s]" +
		"[Blit: " + std::to_string(blitTimed) + "s]" +
		"[FOut: " + std::to_string(frameoutTimed) + "s]" +
		"[Hits: " + std::to_string(r_hitCount) + "] " +
		"[Scale: " + std::to_string(r_scale) + "] " +
		"[Interlacing: " + (r_interlaced ? "On" : "Off") + "] ";

	if (r_paused)
		tText += " *** PAUSED ***";
	lHeight += drawText(0, lHeight, tText);

	// Camera Info
	tText = "Camera: P[" +
		std::to_string(camera->position.x) + ", " +
		std::to_string(camera->position.y) + ", " +
		std::to_string(camera->position.z) + "] D[" +
		std::to_string(camera->angle.x) + ", " +
		std::to_string(camera->angle.y) + ", " +
		std::to_string(camera->angle.z) + "] R[" +
		std::to_string(camera->rot.x) + ", " +
		std::to_string(camera->rot.y) + ", " +
		std::to_string(camera->rot.z) + ", " +
		std::to_string(camera->rot.w) + "]";
	lHeight += drawText(0, lHeight, tText);

	// Thread Info
	if (r_threadInfo) {
		double tTotal = 0.0;
		for (auto &thread : threads) {
			lHeight += drawText(0, lHeight,
				"t[" + std::to_string(thread.tid) + "] " +
					std::to_string(thread.stats.tTime) + "s");
			tTotal += thread.stats.tTime;
		}

		lHeight += drawText(0, lHeight, "Total Thread Time: " + std::to_string(tTotal) + "s");
	}
}

int traceRay(cRay *cameraRay, vec3 *color, unsigned int depth,
	const bool reflectionPass, cRay *reflectRay,
	const bool shadowPass, cRay *shadowRay) {

	// We've gone too far!
	if (depth > r_bounces)
		return 0;

	float depthMod = 1.0f;
	if (depth > 0)
		depthMod = 0.05f;

	float lDist, lAttn, lWeight, specWeight;
	float dist2cam, hitDist;

	cLight *light;
	vec3 hDir, lDir, vDir;

	vec3 sColor, sNormal, specColor;

	cShape *closestShape, *reflectionShape;
	hit_t hit, closestHit, reflectionHit;

	int hitCount = 0;

	// Find the closest shape
	closestShape = nullptr;
	dist2cam = FLT_MAX;
	for (int s = 0; s < scene->shapeCount; ++s) {
		if (scene->shapes[s]->intersect(cameraRay, hit)) {
			hitDist = vec3::length(camera->position - hit.point);
			if (!closestShape || hitDist < dist2cam) {
				closestShape = scene->shapes[s];
				closestHit = hit;
				dist2cam = hitDist;
			}
			++hitCount;
		}
	}

	// Hit something, light it!
	if (closestShape) {
		sNormal = closestShape->getNormal(closestHit);
		sColor = closestShape->getColor(closestHit);

		shadowRay->origin = closestHit.point;

		*color += ambientColor * sColor * depthMod;

		// Trace Lights
		for (int l = 0; l < scene->lightCount; ++l) {
			light = scene->lights[l];
			lDir = light->position - closestHit.point;
			lDist = abs(lDir.length());

			if (lDist > light->size)
				continue;

			lDir.normalise();
			shadowRay->direction = lDir;

			if (shadowPass && inShadow(shadowRay, closestShape, lDist))
				continue;

			lWeight = std::max(vec3::dot(sNormal, lDir), 0.0f);
			lAttn = std::max(std::min(1.0f - lDist / light->size, 1.0f), 0.0f);

			// Specular
			vDir = camera->position - closestHit.point;
			vDir.normalise();
			hDir = lDir + vDir;
			hDir.normalise();
			specWeight = powf(std::min(vec3::dot(sNormal, hDir), 1.0f),
				closestShape->specPower);
			specColor = light->color * specWeight * closestShape->shininess;

			// Light Influance
			*color += (light->color + specColor + sColor) * lAttn * lWeight * depthMod;
		}

		// Reflections
		if (reflectionPass) {
			reflectRay->origin = closestHit.point;
			reflectRay->direction = sNormal;

			reflectionShape = nullptr;
			dist2cam = FLT_MAX;
			for (int s = 0; s < scene->shapeCount; ++s) {
				if (scene->shapes[s] == closestShape)
					continue;

				if (scene->shapes[s]->intersect(reflectRay, hit)) {
					++hitCount;

					hitDist = vec3::length(camera->position - hit.point);
					if (!reflectionShape || hitDist < dist2cam) {
						reflectionShape = scene->shapes[s];
						reflectionHit = hit;
					}
				}
			}

			if (reflectionShape) {
				++depth;
				hitCount += traceRay(reflectRay, color, depth,
					reflectionPass, reflectRay, shadowPass, shadowRay);
			}
		}

	// Hit nothing, trace the viewdir for background
	} else if (depth == 0) {
		color->r += (1.0f + cameraRay->direction.x) * 0.5f;
		color->g += 0.0f;
		color->b += (1.0f + cameraRay->direction.y) * 0.5f;
	}

	*color = vec3::min(*color, 1.0);
	return hitCount;
}

void drawStuffJob(thread_t *tData) {
	cCamera *camera = (cCamera*) tData->camera;
	region_t *region = (region_t*) tData->region;

	uint64_t tTime = 0;
	int pixel;

	vec3 color, dir;

	int rStride = region->width.end - region->width.start;
	tData->hitCount = 0;

	if (r_threadInfo)
		tTime = SDL_GetPerformanceCounter();

	tData->cameraRay->origin = camera->position;
	tData->cameraRay->direction = camera->direction;
	tData->cameraRay->direction.z = -1.0f;

	for (int y = region->height.start; y < region->height.end; ++y) {
		for (int x = region->width.start; x < region->width.end; ++x) {
			pixel = y * rStride + (x - region->width.start);
			region->pixels[pixel] = 0;

			if (r_interlaced && y % 2)
				continue;

			dir.x = (2.0f * (x + 0.5f) / (float) r_width - 1) * r_aspect * camera->viewAngle;
			dir.y = (1.0f - 2.0f * (y + 0.5f) / (float) r_height) * camera->viewAngle;
			dir.z = -1.0f;

			tData->cameraRay->direction = camera->mat * dir;
			tData->cameraRay->direction.normalise();

			color = {0, 0, 0};

			tData->hitCount += traceRay(tData->cameraRay, &color, 0,
				tData->reflections, tData->reflectRay,
				tData->shadows, tData->shadowRay);

			region->pixels[pixel] =
				(Uint32) 255 |
				((unsigned char)(255 * color.b) << 8)  |
				((unsigned char)(255 * color.g) << 16) |
				((unsigned char)(255 * color.r) << 24);
		}
	}

	if (r_threadInfo)
		tData->stats.tTime = (double) (SDL_GetPerformanceCounter() - tTime) / cpuFreq;
}

void setScale(const unsigned int scale) {
	r_scale = scale;
	r_height = win_height / scale;
	r_width = win_width / scale;
	r_aspect = (float) r_width / (float) r_height;
	SDL_RenderSetScale(renderer, (float) r_scale, (float) r_scale);
	resizeRegions();
	r_pass = true;
}

// ----------------------------
// Regions
// ----------------------------
void blitRegions(void) {
	blitTime = SDL_GetPerformanceCounter();

	region_t *region;
	for (unsigned int r = 0; r < r_regions; ++r) {
		region = &regions[r];
		for (int y = region->height.start; y < region->height.end; ++y)
			for (int x = region->width.start; x < region->width.end; ++x)
				frameBuffer[(y * r_width) + x] =
					region->pixels[y * (region->width.end - region->width.start) +
						(x - region->width.start)];
	}

	blitTimed = (double) (SDL_GetPerformanceCounter() - blitTime) /	cpuFreq;
}

void postProcessing(void) {
	// Super shitty Vignette!
	if (r_vignette) {
		int vignette = 30;
		int hWidth = r_width / 2;
		int hHeight = r_height / 2;
		float r, g, b, a, dist;
		int ax, ay;

		Uint32 pixel;
		for (int y = 0; y < r_height; ++y) {
			for (int x = 0; x < r_width; ++x) {

				ax = abs(x - hWidth);
				ay = abs(y - hHeight);
				dist = (float) sqrt(ax * ax + ay * ay);

				if (dist > hWidth) {
					pixel = frameBuffer[(y * r_width) + x];

					a = (float)  (pixel & 0x000000ff) / 255;
					r = (float) ((pixel & 0x0000ff00) >> 8)  / 255;
					g = (float) ((pixel & 0x00ff0000) >> 16) / 255;
					b = (float) ((pixel & 0xff000000) >> 24) / 255;

					float fade = 1.0f - (abs(dist - hWidth) / (vignette * 2));
					fade = std::min(std::max(0.0f, fade), 1.0f);

					r *= fade;
					g *= fade;
					b *= fade;

					frameBuffer[(y * r_width) + x] = (Uint32)
						((unsigned char) a) |
						((unsigned char) (r * 255) <<  8) |
						((unsigned char) (g * 255) << 16) |
						((unsigned char) (b * 255) << 24);
				}
			}
		}
	}
}

void outputFrame(void) {
	frameoutTime = SDL_GetPerformanceCounter();
	int pitch;
	void *pData;

	SDL_LockTexture(blitTexture, NULL, &pData, &pitch);
		memcpy(pData, frameBuffer, pitch * r_height);
	SDL_UnlockTexture(blitTexture);

	SDL_RenderCopy(renderer, blitTexture, NULL, NULL);
	frameoutTimed = (double) (SDL_GetPerformanceCounter() - frameoutTime) /	cpuFreq;

}

void resizeRegions(void) {
	static bool inited = false;

	unsigned int r = 0;
	int widthChunk = (int) ceil((float) r_width / (float) r_regions);
	for (auto &region : regions) {
		region.width.start = r * widthChunk;
		region.width.end = (r * widthChunk) + widthChunk;
		region.height.start = 0;
		region.height.end = r_height;

		// Clamp for bad rounding
		if (region.width.end > r_width)		region.width.end = (int) r_width;
		if (region.height.end > r_height)	region.height.end = (int) r_height;

		if (inited)
			delete region.pixels;

		int rSize = (region.height.end - region.height.start) *
			(region.width.end - region.width.start);
		region.pixels = new Uint32[rSize];
		memset(region.pixels, 0, rSize * sizeof(Uint32));

		++r;
	}

	if (inited)
		delete frameBuffer;
	frameBuffer = new Uint32[r_width * r_height * sizeof(Uint32)];
	memset(frameBuffer, 0, r_width * r_height * sizeof(Uint32));

	if (inited)
		SDL_DestroyTexture(blitTexture);
	blitTexture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,
		r_width, r_height);

	inited = true;
}

//-----------------------------
// Shadows
//-----------------------------
bool inShadow(cRay* ray, cShape *curShape, const float lDist) {
	hit_t hit;
	for (int s = 0; s < scene->shapeCount; ++s) {
		if (scene->shapes[s] == curShape)
			continue;

		if (scene->shapes[s]->intersect(ray, hit) && vec3::length(hit.point - ray->origin) < lDist)
			return true;
	}
	return false;
}
