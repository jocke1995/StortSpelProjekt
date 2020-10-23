#ifndef NAV_MESH_H
#define NAV_MESH_H

#include "Core.h"
#include "EngineMath.h"

struct NavQuad;
struct NavTriangle;

class Entity;
class Scene;

typedef union Connection
{
	struct { NavQuad* quadOne; NavQuad* quadTwo; };
	struct { NavTriangle* triOne; NavTriangle* triTwo; };

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
	NavTriangle* GetConnectedTriangle(NavTriangle* currTri)
	{
		if (triOne == currTri)
		{
			return triTwo;
		}
		else
		{
			return triOne;
		}
	}
} Connection;

struct NavQuad
{
	float3 position;
	float2 size;
	int id;

	std::vector<Connection*> connections;
};

struct NavTriangle
{
	float3 vertex1;
	float3 vertex2;
	float3 vertex3;
	float3 center;
	int id;

	std::vector<Connection*> connections;
};


class NavMesh
{
public:
	NavMesh(Scene* scene, const std::string& type);
	~NavMesh();

	NavQuad* AddNavQuad(float3 position, float2 size);
	NavTriangle* AddNavTriangle(float3 vertex1, float3 vertex2, float3 vertex3);

	void ConnectNavQuads(NavQuad* nav1, NavQuad* nav2);
	void ConnectNavTriangles(NavTriangle* nav1, NavTriangle* nav2);
	void ConnectNavQuads(int nav1, int nav2);
	void ConnectNavTriangles(int nav1, int nav2);

	//Returns a quad for the given position
	NavQuad* GetQuad(float3 position);
	NavTriangle* GetTriangle(float3 position);
	std::vector<NavQuad*> GetAllQuads();
	std::vector<NavTriangle*> GetAllTriangles();
	int GetNumQuads();
	int GetNumTriangles();
	const std::string& GetType();

	void CreateQuadGrid();
	void CreateTriangleGrid();

private:
	std::vector<NavQuad*> m_NavQuads;
	std::vector<NavTriangle*> m_NavTriangles;
	std::vector<Connection*> m_Connections;

	std::string m_Type;

	Entity* m_pEntity;

	int m_NumScenes;

	float triangleArea(float3 vertex1, float3 vertex2, float3 vertex3);
};

#endif