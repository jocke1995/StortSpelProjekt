#include "ConsoleCommand.h"
#include "Misc/Multithreading/ThreadPool.h"
#include "ClientPool.h"

#include <chrono>
#include <iostream>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::vector<sf::TcpSocket*> clients;

	int nrOfClients = 0;

	// ThreadPool
	int numCores = 4;
	ThreadPool* threadPool = &ThreadPool::GetInstance(numCores); // Set num m_Threads to number of cores of the cpu

	Console console;
	sf::SocketSelector selector;

	std::string str = "";

	std::cout << "Write 1 for server or 0 for client" << std::endl;
	std::cin >> str;

	auto start = std::chrono::system_clock::now();
	std::chrono::time_point<std::chrono::system_clock> timeNow;
	std::chrono::time_point<std::chrono::system_clock> timeLast;

	double dt = 0;

	if (str == "1")
	{
		ClientPool server(55555);
		threadPool->AddTask(&console);


		while (true)
			{
				timeLast = timeNow;
				timeNow = std::chrono::system_clock::now();
				std::chrono::duration<double> elapsed_time = timeNow - timeLast;
				dt = elapsed_time.count();
				str = "";
				console.GetInput(&str);

				if (strcmp(str.c_str(), "") != 0)
				{
					threadPool->AddTask(&console);
				}
				if (strcmp(str.c_str(), "AddClient") == 0)
				{
					server.AddClient();
					std::cout << server.GetNrOfClients() << " Client slots in total" << std::endl;
				}
				if (strcmp(str.c_str(), "Packet") == 0)
				{
					server.toggleShowPackage();
				}

				server.ListenMessages();
				server.Update(dt);

				if (strcmp(str.c_str(), "quit") == 0)
				{
					break;
				}

				str = server.GetConsoleString();

				if (str != "")
				{
					std::cout << str;
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

		threadPool->AddTask(&console);

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
				threadPool->AddTask(&console);
			}

			clients.at(0)->receive(packet);
			std::string temp = "";
			packet >> temp;
			if(temp != "")
				std::cout << temp << std::endl;
		}


	}

	return 0;
}