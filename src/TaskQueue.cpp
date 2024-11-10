#include "TaskQueue.h"
#include "DatabaseManager.h"

TaskQueue::TaskQueue() {
    dbManager_.init();
    // Load pending tasks from database
    auto pendingTasks = dbManager_.getPendingTasks();
    for (const auto& task : pendingTasks) {
        tasks_.push(task);
    }
}

void TaskQueue::addTask(const Task& task) {
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.push(task);
    dbManager_.addTask(task);
    condition_.notify_one();
}

Task TaskQueue::getNextTask() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this] { return !tasks_.empty(); });

    Task task = tasks_.front();
    tasks_.pop();
    return task;
}

bool TaskQueue::hasTask() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return !tasks_.empty();
}

void TaskQueue::markTaskCompleted(const Poco::UUID& taskId) {
    Task task = dbManager_.getTask(taskId);
    task.setCompleted(true);
    task.setStatus("COMPLETED");
    dbManager_.updateTaskStatus(taskId, "COMPLETED");
}

void TaskQueue::markTaskCompleted(const Poco::UUID& taskId, const Poco::UUID& workerId) {
    try {
        dbManager_.markTaskCompleted(taskId, workerId);
    }
    catch (const std::exception& e) {
        std::cerr << "Error marking task as completed: " << e.what() << std::endl;
    }
}

void TaskQueue::assignTaskToWorker(const Poco::UUID& taskId, const Poco::UUID& workerId) {
    std::string status = "IN_PROGRESS";
    dbManager_.updateTaskAssignment(taskId, workerId, status);
}