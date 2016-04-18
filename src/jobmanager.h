#ifndef __JOBMAN_H__
#define __JOBMAN_H__

#include <condition_variable>
#include <future>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class jobManager {
	std::vector<std::thread> workers;

	public:
		std::queue<std::function<void()>> jobs;

		bool shutdown;
		std::atomic<int> jobCount;
		std::atomic<bool> running;

		std::condition_variable qCondition;
		std::mutex qMutex;

		std::condition_variable jCondition;
		std::mutex jMutex;

		jobManager(unsigned int wCount = 1);
		~jobManager(void);

		template<class F, class... Args>
		auto addJob(F&& f, Args&&... args) ->
			std::future<typename std::result_of<F(Args...)>::type>;

		void run(void);
};

// Gah, MSVC gets pissy about this being in the cpp file =/
template<class F, class... Args>
auto jobManager::addJob(F&& f, Args&&... args) ->
	std::future<typename std::result_of<F(Args...)>::type> {

	using return_type = typename std::result_of<F(Args...)>::type;

	auto job = std::make_shared<std::packaged_task<return_type()>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
	);

	std::future<return_type> res = job->get_future();

	std::unique_lock<std::mutex> lock(qMutex);
	jobs.emplace([job](){ (*job)(); });
	jobCount++;

	return res;
}

#endif
