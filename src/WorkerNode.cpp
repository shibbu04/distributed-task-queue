// WorkerNode.cpp
#include "WorkerNode.h"
#include <Poco/Net/SocketStream.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <iostream>
#include <chrono>
#include <iomanip>

WorkerNode::WorkerNode(const std::string& serverHost, int serverPort)
    : serverHost_(serverHost)
    , serverPort_(serverPort)
    , running_(false)
    , workerId_(Poco::UUIDGenerator::defaultGenerator().createOne())
    , heartbeatRunnable_(new HeartbeatRunnable(this))
    , currentLoad_(0.0f)
    , rng_(std::random_device{}())
    , loadDist_(-0.1f, 0.1f) {
}

WorkerNode::~WorkerNode() {
    stop();
    delete heartbeatRunnable_;
}

void WorkerNode::start() {
    if (!running_) {
        running_ = true;
        heartbeatThread_.start(*heartbeatRunnable_);
        
        try {
            socket_.connect(Poco::Net::SocketAddress(serverHost_, serverPort_));
            std::cout << "Connected to server at " << serverHost_ << ":" << serverPort_ << std::endl;
            
            // Start task processing thread
            taskThread_ = std::thread([this]() {
                char buffer[4096];
                while (running_) {
                    try {
                        int n = socket_.receiveBytes(buffer, sizeof(buffer));
                        if (n > 0) {
                            std::string message(buffer, n);
                            
                            // Parse task message
                            Poco::JSON::Parser parser;
                            Poco::Dynamic::Var result = parser.parse(message);
                            Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
                            
                            if (object->getValue<std::string>("type") == "new_task") {
                                Poco::JSON::Object::Ptr taskObj = object->getObject("task");
                                Task task(
                                    Poco::UUID(taskObj->getValue<std::string>("id")),
                                    taskObj->getValue<std::string>("name"),
                                    taskObj->getValue<std::string>("data")
                                );
                                processTask(task);
                            }
                        }
                    }
                    catch (const std::exception& e) {
                        std::cerr << "Error processing message: " << e.what() << std::endl;
                    }
                }
            });
        }
        catch (const std::exception& e) {
            std::cerr << "Error connecting to server: " << e.what() << std::endl;
            running_ = false;
            throw;
        }
    }
}

void WorkerNode::stop() {
    if (running_) {
        running_ = false;
        
        if (taskThread_.joinable()) {
            taskThread_.join();
        }
        
        heartbeatThread_.join();
        socket_.close();
    }
}

void WorkerNode::updateLoad() {
    // Simulate load changes with random walk
    float newLoad = currentLoad_ + loadDist_(rng_);
    // Clamp between 0 and 1
    newLoad = std::max(0.0f, std::min(1.0f, newLoad));
    currentLoad_ = newLoad;
}

void WorkerNode::drawStats() const {
    std::cout << "\033[2J\033[H"; // Clear screen and move cursor to top
    std::cout << "╔════════════════════════════════════╗" << std::endl;
    std::cout << "║           Worker Status            ║" << std::endl;
    std::cout << "╠════════════════════════════════════╣" << std::endl;
    std::cout << "║ Worker ID: " << workerId_.toString() << std::endl;
    std::cout << "║ Server: " << serverHost_ << ":" << serverPort_ << std::endl;
    std::cout << "║ Current Load: " << std::fixed << std::setprecision(2) 
              << (currentLoad_ * 100.0f) << "%" << std::endl;
    
    // Visual load bar
    std::cout << "║ Load: [";
    int barWidth = 20;
    int filledWidth = static_cast<int>(currentLoad_ * barWidth);
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) std::cout << "█";
        else std::cout << " ";
    }
    std::cout << "]" << std::endl;
    std::cout << "╚════════════════════════════════════╝" << std::endl;
}

void WorkerNode::processTask(const Task& task) {
    std::cout << "\n╔════════════════════════════════════╗" << std::endl;
    std::cout << "║ Processing Task: " << task.getName() << std::endl;
    std::cout << "║ Task ID: " << task.getId().toString() << std::endl;
    std::cout << "║ Priority: " << task.getPriority() << std::endl;
    std::cout << "╚════════════════════════════════════╝\n" << std::endl;

    // Simulate task processing
    std::this_thread::sleep_for(std::chrono::seconds(2));

    try {
        // Send completion notification to server
        Poco::JSON::Object completionMessage;
        completionMessage.set("type", "task_completed");
        completionMessage.set("task_id", task.getId().toString());
        completionMessage.set("worker_id", workerId_.toString());

        Poco::Net::StreamSocket completionSocket;
        completionSocket.connect(Poco::Net::SocketAddress(serverHost_, serverPort_));
        Poco::Net::SocketStream stream(completionSocket);
        completionMessage.stringify(stream);
        stream.flush();
        completionSocket.close();

        std::cout << "\n╔════════════════════════════════════╗" << std::endl;
        std::cout << "║ Task Completed Successfully!         ║" << std::endl;
        std::cout << "║ Task: " << task.getName() << std::endl;
        std::cout << "╚════════════════════════════════════╝\n" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error sending completion notification: " << e.what() << std::endl;
    }
}

void HeartbeatRunnable::run() {
    while (worker_->isRunning()) {
        try {
            // Update load randomly
            worker_->updateLoad();

            // Create heartbeat message
            Poco::JSON::Object heartbeat;
            heartbeat.set("type", "heartbeat");
            heartbeat.set("worker_id", worker_->workerId_.toString());
            heartbeat.set("load", worker_->getCurrentLoad());

            // Send heartbeat
            Poco::Net::StreamSocket heartbeatSocket;
            heartbeatSocket.connect(Poco::Net::SocketAddress(worker_->serverHost_, worker_->serverPort_));
            Poco::Net::SocketStream stream(heartbeatSocket);
            heartbeat.stringify(stream);
            stream.flush();
            heartbeatSocket.close();

            // Display current stats
            worker_->drawStats();
        }
        catch (const std::exception& e) {
            std::cerr << "Error sending heartbeat: " << e.what() << std::endl;
        }

        Poco::Thread::sleep(1000); // Sleep for 1 second between heartbeats
    }
}

// Main function implementation
int main(int argc, char* argv[]) {
    try {
        std::string serverHost = "localhost";  // Default host
        int serverPort = 8080;                 // Default port

        // Parse command line arguments if provided
        if (argc >= 2) serverHost = argv[1];
        if (argc >= 3) serverPort = std::stoi(argv[2]);

        std::cout << "Starting worker node..." << std::endl;
        std::cout << "Connecting to server at " << serverHost << ":" << serverPort << std::endl;

        WorkerNode worker(serverHost, serverPort);
        worker.start();

        // Wait for Ctrl+C
        std::cout << "Worker node running. Press Ctrl+C to stop." << std::endl;
        while (worker.isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}