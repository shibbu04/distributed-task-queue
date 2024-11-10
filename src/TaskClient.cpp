#include "TaskClient.h"
#include <Poco/Net/StreamSocket.h>

TaskClient::TaskClient(const std::string& host, int port)
    : host_(host)
    , port_(port) {
}

void TaskClient::submitTask(const Task& task) {
    try {
        Poco::Net::SocketAddress address(host_, port_);
        Poco::Net::StreamSocket socket(address);
        Poco::Net::SocketStream stream(socket);

        Poco::JSON::Object json;
        json.set("type", "submit_task");
        
        Poco::JSON::Object taskObj;
        taskObj.set("id", task.getId().toString());
        taskObj.set("name", task.getName());
        taskObj.set("data", task.getData());
        
        json.set("task", taskObj);
        json.stringify(stream);
        stream.flush();
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to submit task: " + std::string(e.what()));
    }
}

bool TaskClient::checkTaskStatus(const Poco::UUID& taskId) {
    try {
        Poco::Net::SocketAddress address(host_, port_);
        Poco::Net::StreamSocket socket(address);
        Poco::Net::SocketStream stream(socket);

        Poco::JSON::Object json;
        json.set("type", "check_status");
        json.set("task_id", taskId.toString());
        
        json.stringify(stream);
        stream.flush();

        // Read response
        char buffer[1024];
        int n = socket.receiveBytes(buffer, sizeof(buffer));
        if (n > 0) {
            std::string response(buffer, n);
            Poco::JSON::Parser parser;
            auto result = parser.parse(response);
            auto object = result.extract<Poco::JSON::Object::Ptr>();
            return object->getValue<bool>("completed");
        }
        return false;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to check task status: " + std::string(e.what()));
    }
}