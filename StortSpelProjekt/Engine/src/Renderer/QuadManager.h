#ifndef QUADMANAGER_H
#define QUADMANAGER_H

#include <map>
#include <vector>

class Texture;
class Mesh;
class Resource;
class ShaderResourceView;
class Eventbus;
class Renderer;

struct Vertex;
struct MouseClick;

static int s_Id = 0;

enum E_DEPTH_LEVEL
{
	BACK,
	MID,
	FRONT,
	NUM_DEPTH_LEVELS
};

class QuadManager
{
public:
	QuadManager();
	~QuadManager();

	bool operator== (const QuadManager& other) const;

	void CreateQuad(
		float2 pos, float2 size, 
		bool clickable, 
		E_DEPTH_LEVEL depthLevel, 
		float4 blend = float4{ 1.0, 1.0, 1.0, 1.0 },
		Texture* texture = nullptr,
		float4 color = float4{ 0.0, 0.0, 0.0, 1.0 });
	void UploadAndExecuteQuadData();

	bool HasBeenPressed();

	Mesh* const GetQuad() const;
	Texture* const GetTexture() const;
	SlotInfo* const GetSlotInfo() const;
	const E_DEPTH_LEVEL* GetDepthLevel() const;
	const float4 GetAmountOfBlend() const;

private:
	friend class AssetLoader;
	friend class Renderer;

	int m_Id = 0;
	float4 m_AmountOfBlend = float4{ 1.0, 1.0, 1.0, 1.0 };
	std::map<std::string, float2> m_Positions = {};
	E_DEPTH_LEVEL m_DepthLevel = {};

	Mesh* m_pQuad = nullptr;
	SlotInfo* m_pSlotInfo = nullptr;
	Texture* m_pQuadTexture = nullptr;
	bool m_Clickable = false;
	bool m_Pressed = false;
	
	void pressed(MouseClick* evnt);
	void uploadQuadData(Renderer* renderer);
};
#endif