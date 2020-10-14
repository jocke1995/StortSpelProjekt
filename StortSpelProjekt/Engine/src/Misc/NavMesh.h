#ifndef NAV_MESH_H
#define NAV_MESH_H

#include "Core.h"
#include "EngineMath.h"

struct NavQuad;
struct Connection;

class Entity;
class Scene;

struct NavQuad
{
	float3 position;
	float2 size;

	std::vector<Connection*> connections;

	Entity* entity;
};

struct Connection
{
	NavQuad* quadOne;
	NavQuad* quadTwo;

	float3 position;

	//Takes the current navQuad to identify which pointer it is, in order to give the other quad
	NavQuad* GetConnectedQuad(NavQuad* currQuad)
	{
		if (quadOne == currQuad)
		{
			return quadTwo;
		}
		else
		{
			return quadOne;
		}
	}
};

class NavMesh
{
public:
	NavMesh();
	~NavMesh();

	NavQuad* AddNavQuad(float3 position, float2 size);

	void ConnectNavQuadsByQuad(NavQuad* nav1, NavQuad* nav2, float3 position);
	void ConnectNavQuadsById(int nav1, int nav2, float3 position);

	//Returns a quad for the given position
	NavQuad* GetQuad(float3 position);

	void ShowInScene(Scene* scene);

private:
	std::vector<NavQuad*> m_NavQuads;
	std::vector<Connection*> m_Connections;

	int m_NumScenes;
};

#endif