#pragma once
#include <string>

class Server
{
public:
    Server(const std::string &host, const std::string &port);

    void run();

private:
private:
    const std::string host;
    const std::string port;
};
