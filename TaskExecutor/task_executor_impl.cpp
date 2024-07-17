#include "task_executor_impl.h"

TaskExecutorImpl::TaskExecutorImpl()
	:m_stop(false)
{
	m_thread = std::thread(&TaskExecutorImpl::schedule, this);
}

TaskExecutorImpl::~TaskExecutorImpl()
{
	m_stop = true;
	m_cv.notify_one();
	m_thread.join();
}

size_t TaskExecutorImpl::post(const Task& task, const Condition& cond)
{
	size_t task_count = 0;
	{
		std::unique_lock<std::mutex> lck(m_mtx);
		m_tasks.emplace_back(task, cond);
		task_count = m_tasks.size();
	}

	m_cv.notify_one();

	return task_count;
}

void TaskExecutorImpl::schedule()
{
	while (!m_stop)
	{
		std::list<Task> tasks;

		std::unique_lock<std::mutex> lck(m_mtx);

		m_cv.wait(lck, [&tasks, this]()->bool
			{
				if (m_stop)
				{
					return true;
				}

				if (m_tasks.empty())
				{
					return false;
				}

				while (!m_tasks.empty())
				{
					const auto& wtask = m_tasks.front();
					const auto& cond = wtask.second;
					if (cond == nullptr || cond())
					{
						tasks.push_back(wtask.first);
						m_tasks.pop_front();
					}
					else
					{
						break;
					}
				}

				return tasks.empty() ? false : true;
			});

		lck.unlock();

		if (!tasks.empty())
		{
			for (auto& task : tasks)
			{
				task();
			}
			tasks.clear();
		}
	}

	// Ö´ÐÐÆ÷Îö¹¹
	while (!m_tasks.empty())
	{
		const auto& wtask = m_tasks.front();
		const auto& cond = wtask.second;
		if (cond == nullptr || cond())
		{
			wtask.first();
			m_tasks.pop_front();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
}

size_t TaskExecutorImpl::task_count()
{
	std::unique_lock<std::mutex> lck(m_mtx);
	return m_tasks.size();
}