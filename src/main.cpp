#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/SocketReactor.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/StreamSocket.h>
#include <Poco/Thread.h>
#include <Poco/Util/ServerApplication.h>
#include <csignal>
#include <memory>
#include "TaskQueue.h"
#include "LoadBalancer.h"
#include "DatabaseManager.h"
#include "TaskDistributor.h"
#include <Poco/StreamCopier.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

namespace {
    volatile sig_atomic_t shouldShutdown = false;
}

void signalHandler(int) {
    shouldShutdown = true;
}

// Function declaration
void printBanner() {
    std::cout << "\n"
              << "╔════════════════════════════════════════╗\n"
              << "║        Distributed Task Queue          ║\n"
              << "║            Server v1.0                 ║\n"
              << "╚════════════════════════════════════════╝\n"
              << std::endl;
}

class TaskServerHandler {
public:
    TaskServerHandler(const TaskServerHandler&) = delete;
    TaskServerHandler& operator=(const TaskServerHandler&) = delete;

    TaskServerHandler(Poco::Net::StreamSocket& socket, 
                     Poco::Net::SocketReactor& reactor,
                     std::shared_ptr<TaskQueue> taskQueue,
                     std::shared_ptr<LoadBalancer> loadBalancer)
        : socket_(socket)
        , reactor_(reactor)
        , taskQueue_(taskQueue)
        , loadBalancer_(loadBalancer) {
        reactor_.addEventHandler(socket_,
            Poco::Observer<TaskServerHandler, Poco::Net::ReadableNotification>
            (*this, &TaskServerHandler::onReadable));
    }

    ~TaskServerHandler() {
        reactor_.removeEventHandler(socket_,
            Poco::Observer<TaskServerHandler, Poco::Net::ReadableNotification>
            (*this, &TaskServerHandler::onReadable));
    }

    void onReadable(Poco::Net::ReadableNotification* pNf) {
        try {
            char buffer[4096];
            int n = socket_.receiveBytes(buffer, sizeof(buffer));
            if (n > 0) {
                std::string message(buffer, n);
                handleMessage(message);
            }
        }
        catch (Poco::Exception& exc) {
            std::cerr << "Error handling connection: " << exc.displayText() << std::endl;
        }
        pNf->release();
    }

private:
    void handleMessage(const std::string& message) {
    try {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(message);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        std::string type = object->getValue<std::string>("type");

        if (type == "task_completed") {
            std::string taskId = object->getValue<std::string>("task_id");
            std::string workerId = object->getValue<std::string>("worker_id");
            taskQueue_->markTaskCompleted(Poco::UUID(taskId), Poco::UUID(workerId));
            loadBalancer_->updateWorkerStatus(Poco::UUID(workerId), true);
        }
        else if (type == "heartbeat") {
            std::string workerId = object->getValue<std::string>("worker_id");
            loadBalancer_->updateWorkerStatus(Poco::UUID(workerId), true);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing message: " << e.what() << std::endl;
    }
}

    Poco::Net::StreamSocket socket_;
    Poco::Net::SocketReactor& reactor_;
    std::shared_ptr<TaskQueue> taskQueue_;
    std::shared_ptr<LoadBalancer> loadBalancer_;
};

class CustomSocketAcceptor {
public:
    CustomSocketAcceptor(Poco::Net::ServerSocket& socket,
                        Poco::Net::SocketReactor& reactor,
                        std::shared_ptr<TaskQueue> taskQueue,
                        std::shared_ptr<LoadBalancer> loadBalancer)
        : socket_(socket)
        , reactor_(reactor)
        , taskQueue_(taskQueue)
        , loadBalancer_(loadBalancer) {
        reactor_.addEventHandler(socket_,
            Poco::Observer<CustomSocketAcceptor,
            Poco::Net::ReadableNotification>
            (*this, &CustomSocketAcceptor::onAccept));
    }

    ~CustomSocketAcceptor() {
        reactor_.removeEventHandler(socket_,
            Poco::Observer<CustomSocketAcceptor,
            Poco::Net::ReadableNotification>
            (*this, &CustomSocketAcceptor::onAccept));
    }

    void onAccept(Poco::Net::ReadableNotification* pNf) {
        try {
            Poco::Net::StreamSocket sock = socket_.acceptConnection();
            new TaskServerHandler(sock, reactor_, taskQueue_, loadBalancer_);
        }
        catch (Poco::Exception& exc) {
            std::cerr << "Error accepting connection: " << exc.displayText() << std::endl;
        }
        pNf->release();
    }

private:
    Poco::Net::ServerSocket& socket_;
    Poco::Net::SocketReactor& reactor_;
    std::shared_ptr<TaskQueue> taskQueue_;
    std::shared_ptr<LoadBalancer> loadBalancer_;
};

class TaskServer {
public:
    TaskServer() : port_(8080) {
        taskQueue_ = std::make_shared<TaskQueue>();
        loadBalancer_ = std::make_shared<LoadBalancer>();
        taskDistributor_ = std::make_shared<TaskDistributor>(taskQueue_, loadBalancer_);
    }

    void start() {
        try {
            Poco::Net::ServerSocket serverSocket(port_);
            Poco::Net::SocketReactor reactor;

            CustomSocketAcceptor acceptor(
                serverSocket, reactor, taskQueue_, loadBalancer_);

            taskDistributor_->start();
            std::cout << "Server started on port " << port_ << std::endl;

            Poco::Thread thread;
            thread.start(reactor);

            while (!shouldShutdown) {
                Poco::Thread::sleep(100);
            }

            std::cout << "Shutting down server..." << std::endl;
            taskDistributor_->stop();
            reactor.stop();
            thread.join();
        }
        catch (const Poco::Exception& exc) {
            std::cerr << "Error starting server: " << exc.displayText() << std::endl;
            throw;
        }
    }

private:
    int port_;
    std::shared_ptr<TaskQueue> taskQueue_;
    std::shared_ptr<LoadBalancer> loadBalancer_;
    std::shared_ptr<TaskDistributor> taskDistributor_;
};

int main() {
        printBanner();
    try {
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        DatabaseManager dbManager;
        if (!dbManager.init()) {
            std::cerr << "Failed to initialize database. Exiting..." << std::endl;
            return 1;
        }

        TaskServer server;
        server.start();
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}