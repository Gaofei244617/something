#pragma once

#include <utility>
#include <list>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include "task_executor.h"

class TaskExecutorImpl : public TaskExecutor
{
public:
	TaskExecutorImpl();
	~TaskExecutorImpl();

	size_t post(const Task& task, const Condition& cond) override;
	size_t task_count() override;

protected:
	void schedule();

protected:
	std::list<std::pair<Task, Condition>> m_tasks;
	std::thread m_thread;
	std::mutex m_mtx;
	std::condition_variable m_cv;
	std::atomic<bool> m_stop;
};
