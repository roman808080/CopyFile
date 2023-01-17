#pragma once
#include <string>

class Client
{
public:
    Client(const std::string &host, const std::string &port);

    void setClientName(const std::string& clientName);
    void run();

private:
private:
    const std::string host;
    const std::string port;
    std::string clientName;
};

