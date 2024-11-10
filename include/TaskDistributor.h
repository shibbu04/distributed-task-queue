#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include "TaskQueue.h"
#include "LoadBalancer.h"

class TaskDistributor {
public:
    TaskDistributor(std::shared_ptr<TaskQueue> taskQueue, 
                   std::shared_ptr<LoadBalancer> loadBalancer);
    ~TaskDistributor();

    void start();
    void stop();

private:
    void distributeTasks();
    
    std::shared_ptr<TaskQueue> taskQueue_;
    std::shared_ptr<LoadBalancer> loadBalancer_;
    std::atomic<bool> running_;
    std::thread distributor_thread_;
};