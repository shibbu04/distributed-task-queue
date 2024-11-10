#pragma once
#include <Poco/Data/SessionPool.h>
#include "Task.h"
#include <vector>

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool init();
    void addTask(const Task& task);
    void addSampleTasks();
    std::vector<Task> getCompletedTasks();
    void updateTaskStatus(const Poco::UUID& taskId, const std::string& status);
    std::vector<Task> getPendingTasks();
    Task getTask(const Poco::UUID& id);
    void updateTaskAssignment(const Poco::UUID& taskId, const Poco::UUID& workerId, const std::string& status);
    void markTaskCompleted(const Poco::UUID& taskId, const Poco::UUID& workerId);
    void addCompletedTask(const Task& task, const std::string& workerId, const Poco::DateTime& completedAt);

private:
    Poco::Data::SessionPool* sessionPool_;
    static const std::string CONNECTION_STRING;
};
