#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Task.h"
#include "DatabaseManager.h"

class TaskQueue {
public:
    TaskQueue();
    void addTask(const Task& task);
    void markTaskCompleted(const Poco::UUID& taskId, const Poco::UUID& workerId);
    void assignTaskToWorker(const Poco::UUID& taskId, const Poco::UUID& workerId);
    Task getNextTask();
    bool hasTask() const;
    void markTaskCompleted(const Poco::UUID& taskId);

private:
    std::queue<Task> tasks_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    DatabaseManager dbManager_;
};