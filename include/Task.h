#pragma once
#include <string>
#include <Poco/UUID.h>

class Task {
public:
    Task(const std::string& name, const std::string& data);
    Task(const Poco::UUID& id, const std::string& name, const std::string& data);

    Poco::UUID getId() const;
    std::string getName() const;
    std::string getData() const;
    int getPriority() const;
    void setPriority(int priority);
    std::string getStatus() const;
    void setStatus(const std::string& status);
    bool isCompleted() const;
    void setCompleted(bool completed);
    
private:
    Poco::UUID id_;
    std::string name_;
    std::string data_;
    int priority_;
    std::string status_;
    bool completed_;
};