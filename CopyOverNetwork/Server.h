#include <string>

class Server
{
public:
    Server(const std::string &host, const std::string &port);
    ~Server();

    void run();

private:
private:
    const std::string host;
    const std::string port;
};