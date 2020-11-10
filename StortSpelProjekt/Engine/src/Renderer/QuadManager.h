#ifndef QUADMANAGER_H
#define QUADMANAGER_H

#include <map>
#include <vector>

class Texture;
class Mesh;
class Resource;
class ShaderResourceView;
class EventBus;
class Renderer;

struct Vertex;
struct MouseClick;

static int s_Id = 0;

class QuadManager
{
public:
	QuadManager();
	~QuadManager();

	bool operator== (const QuadManager& other) const;

	// If you don't want any texture, send a nullptr
	void CreateQuad(
		std::string name,
		float2 pos, float2 size, 
		bool clickable, bool markable,
		int depthLevel, 
		float4 blend = float4{ 1.0, 1.0, 1.0, 1.0 },
		Texture* texture = nullptr,
		float3 color = float3{ 1.0, 1.0, 1.0 });
	void UpdateQuad(
		float2 pos, float2 size,
		bool clickable, bool markable,
		float4 blend = float4{ 1.0, 1.0, 1.0, 1.0 },
		float3 color = float3{ 1.0, 1.0, 1.0 });

	const bool HasTexture() const;
	const bool IsMarked() const;
	const bool IsClickable() const; 
	const bool IsMarkable() const;
	const bool IsQuadHidden() const;

	Mesh* const GetQuad() const;
	Texture* const GetTexture(bool texture) const;
	SlotInfo* const GetSlotInfo() const;

	const float4 GetAmountOfBlend() const;
	const int GetId() const;
	int GetDepth() const;
	float2 GetScale() const;
	float2 GetPos() const;

	// 0 (false) for the normal texture and 1 (true) for the marked texture
	const bool GetActiveTexture() const;

	// 0 (false) for the normal texture and 1 (true) for the marked texture
	void SetActiveTexture(const bool texture);

	void HideQuad(bool hide);

	//Set the function to be called when this quad is clicked.
	void SetOnClicked(void (*clickFunc)(const std::string&));

private:
	friend class AssetLoader;
	friend class Renderer;

	int m_Id = 0;
	int m_Depth = 0;
	std::string m_Name = "";
	float4 m_AmountOfBlend = { 1.0, 1.0, 1.0, 1.0 };
	float2 m_Scale = { 0.1, 0.1 };
	float2 m_Pos = { 0.0, 0.0 };

	std::map<std::string, float2> m_Positions = {};
	Mesh* m_pQuad = nullptr;
	SlotInfo* m_pSlotInfo = nullptr;
	Texture* m_pQuadTexture = nullptr;
	Texture* m_pQuadTextureMarked = nullptr;
	bool m_Clickable = false;
	bool m_Markable = false;
	bool m_ActiveTexture = false;
	bool m_QuadIsHidden = false;
	std::vector<Mesh*> m_TrashBuffer = {};
	// Needs to take in the name of the button.
	void (*m_pOnClicked)(const std::string &);
	
	void pressed(MouseClick* evnt);
	void uploadQuadData(Renderer* renderer);
	void deleteQuadData();
};
#endif