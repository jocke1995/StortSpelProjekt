#ifndef SERVERGAME_H
#define SERVERGAME_H

#include "SFML/Network.hpp"
#include <vector>

struct ServerEntity
{
	std::string name;
	float3 position;
	double4 rotation;
	double3 velocity;
};

struct EnemyEntity : ServerEntity
{
	int targetId;
};

class ServerGame
{
public:
	ServerGame();
	~ServerGame();

	void StartGameState();

	void Update(double dt);

	void UpdateEntity(std::string name, float3 position, double4 rotation, double3 velocity);
	void UpdateEnemy(std::string name, float3 position, double4 rotation, double3 velocity, int target);
	ServerEntity* GetEntity(std::string name);
	EnemyEntity* GetEnemy(std::string name);

	void AddEntity(std::string name);
	void AddEnemy(std::string name, float3 position, double4 rotation, double3 velocity, int target);
	void RemoveEntity(std::string name);

private:
	int m_FrameCount;

	std::vector<ServerEntity*> m_Entities;
	std::vector<EnemyEntity*> m_Enemies;
};

#endif
