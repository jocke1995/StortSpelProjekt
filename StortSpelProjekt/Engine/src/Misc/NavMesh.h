#ifndef NAV_MESH_H
#define NAV_MESH_H

struct NavQuad
{
	float3 position;
	float2 size;

	std::vector<Connection*> connections;
};

struct Connection
{
	NavQuad* quadOne;
	NavQuad* quadTwo;

	float3 position;

	//Takes the current navQuad to identify which pointer it is, in order to give the other quad
	NavQuad* GetQuad(NavQuad* currQuad)
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

	void AddNavQuad(float3 position, float2 size);

private:
	std::vector<NavQuad> m_NavQuads;
	std::vector<Connection> m_Connections;
};

#endif