#ifndef EDGE_H
#define EDGE_H

#include <vector>

class Entity;

struct NavTriangle;

class Edge
{
public:
	Edge(unsigned int id);
	~Edge();

	void AddEntity(Entity* ent);
	void RemoveEntitiesFromWorld();

	void ConnectToWall(Edge* wallToConnectTo);
	bool IsConnected();

	Edge* GetConnectedWall();
	unsigned int GetId();

	void AddNavTriangle(NavTriangle* tri);
	NavTriangle* GetNavTriangle();

private:
	unsigned int m_Id;
	Edge* m_pConnectedWall;

	std::vector<Entity*> m_Entities;
	NavTriangle* m_pNavTriangle;
};

#endif //EDGE_H