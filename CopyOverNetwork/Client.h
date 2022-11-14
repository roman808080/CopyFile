#include <string>

class Client
{
public:
    Client(const std::string &host, const std::string &port);
    ~Client();

    void run();

private:
private:
    const std::string host;
    const std::string port;
};

