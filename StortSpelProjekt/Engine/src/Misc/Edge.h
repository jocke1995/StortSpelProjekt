#ifndef EDGE_H
#define EDGE_H

#include <vector>

class Entity;

struct NavTriangle;
struct NavMesh;

class Edge
{
public:
	Edge(unsigned int id);
	~Edge();

	void AddEntity(Entity* ent);
	void RemoveEntitiesFromWorld();

	void ConnectToWall(Edge* wallToConnectTo, NavMesh* navMesh);
	bool IsConnected();

	Edge* GetConnectedWall();
	unsigned int GetId();

	void AddNavTriangle(NavTriangle* tri);
	NavTriangle* GetNavTriangle(unsigned int id);
	int GetNumTriangles();

private:
	unsigned int m_Id;
	Edge* m_pConnectedWall;

	std::vector<Entity*> m_Entities;
	std::vector<NavTriangle*> m_NavTriangles;
};

#endif //EDGE_H