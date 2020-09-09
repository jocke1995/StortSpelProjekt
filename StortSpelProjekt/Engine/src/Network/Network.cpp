#include "stdafx.h"
#include "Network.h"

Network::Network()
{
}

bool Network::ConnectToIP(std::string ip, int port)
{
    sf::Socket::Status status = socket.connect(ip, port);

    //if (status != sf::Socket::Done()) {
        Log::PrintSeverity(Log::Severity::WARNING, "Connection to " + ip + " failed");
        return false;
    //}
    //else {
        Log::Print("Connected to " + ip);
        return true;
    //}
}

void Network::ListenConnection(int port)
{
    // bind the listener to a port
    if (listener.listen(port) != sf::Socket::Done)
    {
        // error...
    }

    if (listener.accept(socket) != sf::Socket::Done)
    {
        // error...
    }
}
