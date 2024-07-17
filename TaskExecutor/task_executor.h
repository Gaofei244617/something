#pragma once

#include <memory>
#include <functional>

class TaskExecutor
{
public:
	using Task = std::function<void()>;
	using Condition = std::function<bool()>;

public:
	virtual size_t post(const Task& task, const Condition& cond) = 0;
	virtual size_t task_count() = 0;

public:
	static std::shared_ptr<TaskExecutor> New();
};
