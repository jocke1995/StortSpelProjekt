#include "stdafx.h"
#include "Network.h"

Network::Network()
{
}

bool Network::ConnectToIP(std::string ip, int port)
{
    sf::Socket::Status status = socket.connect(ip, port);

    if (status != 0) 
    {
        Log::PrintSeverity(Log::Severity::WARNING, "Connection to " + ip + " failed\n");
        return false;
    }
    else {
        Log::Print("Connected to " + ip + "\n");
        return true;
    }
}

void Network::ListenConnection(int port)
{
    // bind the listener to a port
    if (listener.listen(port) != sf::Socket::Done)
    {
        Log::PrintSeverity(Log::Severity::WARNING, "Failed attempting to listen to port: " + std::to_string(port) + " failed\n");
    }

    if (listener.accept(socket) != sf::Socket::Done)
    {
        Log::PrintSeverity(Log::Severity::WARNING, "Failed connection to " + socket.getRemoteAddress().toString() + "\n");
    }

    Log::Print("Connected to " + socket.getRemoteAddress().toString() + "\n");
}
