#include "TaskDistributor.h"
#include <Poco/Net/SocketStream.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/JSON/Object.h>
#include <iostream>

TaskDistributor::TaskDistributor(std::shared_ptr<TaskQueue> taskQueue, 
                               std::shared_ptr<LoadBalancer> loadBalancer)
    : taskQueue_(taskQueue)
    , loadBalancer_(loadBalancer)
    , running_(false) {
}

TaskDistributor::~TaskDistributor() {
    stop();
}

void TaskDistributor::start() {
    running_ = true;
    distributor_thread_ = std::thread(&TaskDistributor::distributeTasks, this);
}

void TaskDistributor::stop() {
    running_ = false;
    if (distributor_thread_.joinable()) {
        distributor_thread_.join();
    }
}

void TaskDistributor::distributeTasks() {
    while (running_) {
        if (taskQueue_->hasTask()) {
            Worker* worker = loadBalancer_->getNextAvailableWorker();
            if (worker) {
                try {
                    Task task = taskQueue_->getNextTask();
                    
                    // Update assigned worker in database
                    taskQueue_->assignTaskToWorker(task.getId(), worker->getId());

                    // Create task message
                    Poco::JSON::Object taskMessage;
                    taskMessage.set("type", "new_task");

                    Poco::JSON::Object taskObj;
                    taskObj.set("id", task.getId().toString());
                    taskObj.set("name", task.getName());
                    taskObj.set("data", task.getData());
                    taskObj.set("priority", task.getPriority());  // Include priority in message

                    taskMessage.set("task", taskObj);

                    // Send task to worker
                    Poco::Net::StreamSocket socket;
                    socket.connect(worker->getAddress());
                    Poco::Net::SocketStream stream(socket);
                    taskMessage.stringify(stream);
                    stream.flush();
                    socket.close();

                    worker->setAvailable(false);
                }
                catch (const std::exception& e) {
                    std::cerr << "Error distributing task: " << e.what() << std::endl;
                    loadBalancer_->removeWorker(worker->getId());
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}