#pragma once
#include <string>

class Client
{
public:
    Client(const std::string &host, const std::string &port);

    void setClientName(const std::string& clientName);
    void setSource(const std::string& source);
    void setDestination(const std::string& destination);

    void run();

private:
private:
    const std::string host;
    const std::string port;

    std::string clientName;
    std::string source;
    std::string destination;
};

