#pragma once
#include <vector>
#include <mutex>
#include "Worker.h"

class LoadBalancer {
public:
    void addWorker(const Worker& worker);
    void removeWorker(const Poco::UUID& workerId);
    Worker* getNextAvailableWorker();
    void updateWorkerStatus(const Poco::UUID& workerId, bool available);

private:
    std::vector<Worker> workers_;
    mutable std::mutex mutex_;
    size_t currentWorkerIndex_ = 0;
};