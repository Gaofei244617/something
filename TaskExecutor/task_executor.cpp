#include "task_executor_impl.h"

std::shared_ptr<TaskExecutor> TaskExecutor::New()
{
	return std::make_shared<TaskExecutorImpl>();
}