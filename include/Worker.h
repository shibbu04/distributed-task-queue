#pragma once
#include <string>
#include <Poco/Net/SocketAddress.h>
#include <Poco/UUID.h>
#include <ctime>

class Worker {
public:
    Worker(const std::string& address, int port);
    
    Poco::UUID getId() const;
    Poco::Net::SocketAddress getAddress() const;
    bool isAvailable() const;
    void setAvailable(bool available);
    void updateLastHeartbeat();
    bool isAlive() const;

private:
    Poco::UUID id_;
    Poco::Net::SocketAddress address_;
    bool available_;
    std::time_t lastHeartbeat_;
    static constexpr int HEARTBEAT_TIMEOUT = 30; // seconds
};