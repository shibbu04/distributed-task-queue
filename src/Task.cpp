// Task.cpp
#include "Task.h"
#include <Poco/UUIDGenerator.h>

Task::Task(const std::string& name, const std::string& data)
    : name_(name)
    , data_(data)
    , priority_(1)
    , status_("PENDING")
    , completed_(false) {
    id_ = Poco::UUIDGenerator::defaultGenerator().createOne();
}

Task::Task(const Poco::UUID& id, const std::string& name, const std::string& data)
    : id_(id)
    , name_(name)
    , data_(data)
    , priority_(1)
    , status_("PENDING")
    , completed_(false) {
}

Poco::UUID Task::getId() const { return id_; }
std::string Task::getName() const { return name_; }
std::string Task::getData() const { return data_; }
int Task::getPriority() const { return priority_; }
void Task::setPriority(int priority) { priority_ = priority; }
std::string Task::getStatus() const { return status_; }
void Task::setStatus(const std::string& status) { status_ = status; }
bool Task::isCompleted() const { return completed_; }
void Task::setCompleted(bool completed) { 
    completed_ = completed;
    status_ = completed ? "COMPLETED" : "PENDING";
}