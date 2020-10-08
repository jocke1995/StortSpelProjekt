#ifndef SERVERGAME_H
#define SERVERGAME_H

#include "SFML/Network.hpp"
#include <vector>

struct ServerEntity
{
	std::string name;
	float3 position;
	float3 rotation;
	float3 velocity;
};

class ServerGame
{
public:
	ServerGame();

	void StartGameState();

	void Update(double dt);

	void UpdateEntity(std::string name, float3 position, float3 rotation, float3 velocity);
	ServerEntity* GetEntity(std::string name);

private:
	int m_FrameCount;

	std::vector<ServerEntity> m_Entities;
};

#endif
