//    ___       _    ___  ___
//   |_  |     | |   |  \/  |
//     | | ___ | |__ | .  . | __ _ _ __   __ _  __ _  ___ _ __
//     | |/ _ \| '_ \| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
// /\__/ / (_) | |_) | |  | | (_| | | | | (_| | (_| |  __/ |
// \____/ \___/|_.__/\_|  |_/\__,_|_| |_|\__,_|\__, |\___|_|
//                                              __/ |        Class
// 	                                            |___/

#include "main.h"
#include "jobmanager.h"

jobManager::jobManager(unsigned int wCount) {
	running  = false;
	shutdown = false;
	jobCount = 0;

	for(unsigned int w = 0; w < wCount; ++w) {
		workers.emplace_back([this, w] {
			printf("[Jobs] New Worker[%d] Spawned\n", w);

			while (!shutdown) {
				std::function<void()> job;

				{
					// Wait for something to do
					std::unique_lock<std::mutex> qLock(qMutex);
					this->qCondition.wait(qLock, [this, w]{ return running || shutdown; });

					if (jobs.empty())
						continue;

					job = std::move(jobs.front());
					jobs.pop();
				}

				// Do it!
				job();

				// Finished all jobs, report back
				if (--jobCount == 0) {
					running = false;
					jCondition.notify_one();
				}
			}
		});
	}
}

jobManager::~jobManager(void) {
	printf("[Jobs] Shutting down workers\n");

	shutdown = true;
	qCondition.notify_all();
	for (std::thread &worker: workers)
		worker.join();
}

// ----------------------------
// Methods
// ----------------------------
void jobManager::run(void) {
	if (jobCount < 1)
		return;

	running = true;
	qCondition.notify_all();
}
