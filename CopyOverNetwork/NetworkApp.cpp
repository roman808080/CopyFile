#include "NetworkApp.h"
#include "Server.h"
#include "Client.h"

NetworkApp::NetworkApp(const size_t blockSize)
    : blockSize(blockSize),
      isServer(false)
{
}

void NetworkApp::run()
{
    if (isServer)
    {
        Server server {"0.0.0.0", "5555"};
        server.run();
        return;
    }

    Client client {"0.0.0.0", "5555"};

    client.setClientName(clientName);
    client.setSource(inputFileName);
    client.setDestination(outputFileName);

    client.run();
}

void NetworkApp::setClientName(const std::string &clientName)
{
    this->clientName = clientName;
}

void NetworkApp::setInputFile(const std::string &inputFileName)
{
    this->inputFileName = inputFileName;
}

void NetworkApp::setOutputFile(const std::string &outputFileName)
{
    this->outputFileName = outputFileName;
}

void NetworkApp::setIsServer(bool isServer)
{
    this->isServer = isServer;
}
