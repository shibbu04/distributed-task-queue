#include "Worker.h"
#include <Poco/UUIDGenerator.h>
#include <ctime>  // Add this include

Worker::Worker(const std::string& address, int port)
    : address_(address, port)
    , available_(true) {
    id_ = Poco::UUIDGenerator::defaultGenerator().createOne();
    updateLastHeartbeat();
}

Poco::UUID Worker::getId() const {
    return id_;
}

Poco::Net::SocketAddress Worker::getAddress() const {
    return address_;
}

bool Worker::isAvailable() const {
    return available_;
}

void Worker::setAvailable(bool available) {
    available_ = available;
}

void Worker::updateLastHeartbeat() {
    lastHeartbeat_ = std::time(nullptr);
}

bool Worker::isAlive() const {
    return (std::time(nullptr) - lastHeartbeat_) < HEARTBEAT_TIMEOUT;
}