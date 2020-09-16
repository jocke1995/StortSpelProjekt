#include "Engine.h"
#include "ConsoleCommand.h"
#include "ThreadPool.h"
#include "Thread.h"
#include "MultiThreadedTask.h"

#include <iostream>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::vector<sf::TcpSocket*> clients;

	int nrOfClients = 0;

	// ThreadPool
	int numCores = std::thread::hardware_concurrency();
	if (numCores == 0)
	{
		numCores = 1; // function not supported
	}
	ThreadPool* threadPool = new ThreadPool(numCores); // Set num m_Threads to number of cores of the cpu

	Console console;
	sf::SocketSelector selector;

	std::string str = "";

	std::cout << "Write 1 for server or 0 for client" << std::endl;
	std::cin >> str;

	if (str == "1")
	{
		sf::TcpListener listener;
		// bind the listener to a port
		if (listener.listen(55555) != sf::Socket::Done)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Failed attempting to listen to port: " + std::to_string(55555) + " failed\n");
		}
		selector.add(listener);

		threadPool->AddTask(&console, FLAG_THREAD::NETWORK);

		while (strcmp(str.c_str(), "quit") != 0)
		{
			str = "";
			console.GetInput(&str);

			if (strcmp(str.c_str(), "") != 0)
			{
				std::cout << str << std::endl;
				threadPool->AddTask(&console, FLAG_THREAD::NETWORK);
			}
			if (strcmp(str.c_str(), "AddClient") == 0)
			{
				clients.push_back(new sf::TcpSocket);
			}

			if (selector.wait(sf::seconds(0.1f)))
			{
				if (selector.isReady(listener))
				{
					// Accept connection
					if (listener.accept(*clients.at(clients.size() - 1)) != sf::Socket::Done)
					{
						Log::PrintSeverity(Log::Severity::WARNING, "Failed connection to " + clients.at(clients.size() - 1)->getRemoteAddress().toString() + "\n");
					}
					std::cout << "Connected to " + clients.at(clients.size() - 1)->getRemoteAddress().toString() << std::endl;
					selector.add(*clients.at(clients.size() - 1));
				}
				else
				{
					for (int i = 0; i < clients.size(); i++)
					{
						if (selector.isReady(*clients.at(i)))
						{
							sf::Packet packet;
							std::string temp;
							clients.at(i)->receive(packet);
							packet >> temp;
							std::cout << temp << std::endl;
							for (int j = 0; j < clients.size(); j++) 
							{
								if (j != i)
								{
									clients.at(j)->send(packet);
								}
							}
						}
					}
				}
			}
		}
	}
	else
	{
		std::cout << "Write ip to connect to" << std::endl;
		std::cin >> str;

		clients.push_back(new sf::TcpSocket);

		clients.at(0)->connect(str, 55555);
		clients.at(0)->setBlocking(false);

		threadPool->AddTask(&console, FLAG_THREAD::NETWORK);

		sf::Packet packet;

		while (strcmp(str.c_str(), "quit") != 0)
		{
			str = "";
			console.GetInput(&str);

			if (strcmp(str.c_str(), "") != 0)
			{
				packet.clear();
				packet << str;
				clients.at(0)->send(packet);
				threadPool->AddTask(&console, FLAG_THREAD::NETWORK);
			}

			clients.at(0)->receive(packet);
			std::string temp = "";
			packet >> temp;
			if(temp != "")
				std::cout << temp << std::endl;
		}


	}

	threadPool->ExitThreads();
	delete threadPool;

	return 0;
}