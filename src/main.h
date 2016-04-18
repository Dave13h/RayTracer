#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>

#include <cmath>
#include <math.h>
#include <time.h>
#include <float.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <cctype>

#include <SDL.h>
#include <SDL_cpuinfo.h>
#include <SDL_ttf.h>

#define PI_F 3.14159265358979f
#define DEG2RAD(DEG) ((DEG)*((PI_F)/(180.0f)))

// SDL! Stahp SDL!
#ifdef _WIN32
#undef main
#endif

#endif
