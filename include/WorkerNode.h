#pragma once

#include "Task.h"  // Add this include
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Thread.h>
#include <string>
#include <thread>
#include <atomic>
#include <random>

class WorkerNode;

class HeartbeatRunnable : public Poco::Runnable {
public:
    explicit HeartbeatRunnable(WorkerNode* worker) : worker_(worker) {}
    void run() override;
private:
    WorkerNode* worker_;
};

class WorkerNode {
    friend class HeartbeatRunnable;
public:
    WorkerNode(const std::string& serverHost, int serverPort);
    ~WorkerNode();

    void start();
    void stop();
    bool isRunning() const { return running_; }
    void processTask(const Task& task);  // Now Task is properly declared
    float getCurrentLoad() const { return currentLoad_; }
    void drawStats() const;

private:
    void updateLoad();

    std::string serverHost_;
    int serverPort_;
    std::atomic<bool> running_;
    Poco::UUID workerId_;
    Poco::Net::StreamSocket socket_;
    Poco::Thread heartbeatThread_;
    HeartbeatRunnable* heartbeatRunnable_;
    std::thread taskThread_;
    std::atomic<float> currentLoad_;
    std::mt19937 rng_;
    std::uniform_real_distribution<float> loadDist_;
};