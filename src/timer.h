#ifndef __TIMER_H__
	#define __TIMER_H__

	#include "main.h"

	class cTimer {
		public:
			enum timerState_t {
				STOPPED,
				RUNNING,
				PAUSED
			};

			timerState_t state;
			uint64_t now, prev, cpuFreq;
			float dt; // Delta

			cTimer (const cTimer::timerState_t tState);
			~cTimer (void);

			// Methods
			cTimer *start(void);
			cTimer *stop(void);
			cTimer *tick(void);
	};
#endif
