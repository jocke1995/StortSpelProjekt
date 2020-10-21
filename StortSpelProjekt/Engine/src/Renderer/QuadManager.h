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

	// If you don't want any texture, send a nullptr
	void CreateQuad(
		float2 pos, float2 size, 
		bool clickable, bool markable,
		E_DEPTH_LEVEL depthLevel, 
		float4 blend = float4{ 1.0, 1.0, 1.0, 1.0 },
		Texture* texture = nullptr,
		float3 color = float3{ 0.0, 0.0, 0.0 });
	void UploadAndExecuteQuadData();

	bool HasBeenPressed();
	const bool HasTexture() const;
	const bool IsMarked();
	const bool IsClickable() const; 
	const bool IsMarkable() const;

	Mesh* const GetQuad() const;
	Texture* const GetTexture(bool texture) const;
	SlotInfo* const GetSlotInfo() const;
	const E_DEPTH_LEVEL* GetDepthLevel() const;
	const float4 GetAmountOfBlend() const;

	// 0 (false) for the normal texture and 1 (true) for the marked texture
	const bool GetActiveTexture() const;

	// 0 (false) for the normal texture and 1 (true) for the marked texture
	void SetActiveTexture(const bool texture);

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
	Texture* m_pQuadTextureMarked = nullptr;
	bool m_Clickable = false;
	bool m_Markable = false;
	bool m_Pressed = false;
	bool m_ActiveTexture = false;
	
	void pressed(MouseClick* evnt);
	void uploadQuadData(Renderer* renderer);
};
#endif