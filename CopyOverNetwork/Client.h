#pragma once
#include <string>

class Client
{
public:
    Client(const std::string &host, const std::string &port);

    void run();

private:
private:
    const std::string host;
    const std::string port;
};

