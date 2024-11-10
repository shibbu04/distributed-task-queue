#pragma once
#include <string>
#include <Poco/Net/SocketStream.h>
#include <Poco/JSON/Object.h>
#include "Task.h"

class TaskClient {
public:
    TaskClient(const std::string& host, int port);
    void submitTask(const Task& task);
    bool checkTaskStatus(const Poco::UUID& taskId);

private:
    std::string host_;
    int port_;
};