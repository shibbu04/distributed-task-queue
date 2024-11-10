#include "LoadBalancer.h"

void LoadBalancer::addWorker(const Worker& worker) {
    std::lock_guard<std::mutex> lock(mutex_);
    workers_.push_back(worker);
}

void LoadBalancer::removeWorker(const Poco::UUID& workerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    workers_.erase(
        std::remove_if(workers_.begin(), workers_.end(),
            [&](const Worker& w) { return w.getId() == workerId; }),
        workers_.end()
    );
}

Worker* LoadBalancer::getNextAvailableWorker() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (workers_.empty()) {
        return nullptr;
    }

    // Round-robin selection
    for (size_t i = 0; i < workers_.size(); ++i) {
        currentWorkerIndex_ = (currentWorkerIndex_ + 1) % workers_.size();
        if (workers_[currentWorkerIndex_].isAvailable() && 
            workers_[currentWorkerIndex_].isAlive()) {
            return &workers_[currentWorkerIndex_];
        }
    }
    
    return nullptr;
}

void LoadBalancer::updateWorkerStatus(const Poco::UUID& workerId, bool available) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& worker : workers_) {
        if (worker.getId() == workerId) {
            worker.setAvailable(available);
            break;
        }
    }
}