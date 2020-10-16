#ifndef QUADMANAGER_H
#define QUADMANAGER_H

#include <map>
#include <vector>

class Texture;
class Mesh;
class Resource;
class ShaderResourceView;
class Eventbus;

struct Vertex;
struct MouseClick;

static int s_Id = 0;

class QuadManager
{
	public:
		QuadManager();
		~QuadManager();

		bool operator== (const QuadManager& other) const;

		void CreateQuad(float2 pos, float2 size, bool clickable, std::wstring texturePath = L"NONE");

		bool HasBeenPressed();

		Texture* const GetTexture() const;
	
	private:
		int m_Id = 0;
		std::map<std::string, float2> m_Positions = {};
		Mesh* m_pQuad = nullptr;
		Texture* m_pQuadTexture = nullptr;
		bool m_Clickable = false;
		bool m_Pressed = false;
	
		void pressed(MouseClick* evnt);
};
#endif