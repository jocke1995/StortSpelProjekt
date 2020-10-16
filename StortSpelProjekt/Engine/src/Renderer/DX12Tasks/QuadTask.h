#ifndef QUADTASK_H
#define QUADTASK_H

#include "RenderTask.h"

class RootSignature;
class QuadManager;

enum E_DEPTH_LEVEL;
#include <map>
#include <vector>

namespace component
{
	class GUI2DComponent;
}

class QuadTask : public RenderTask
{
public:
	QuadTask(ID3D12Device5* device,
		RootSignature* rootSignature,
		const std::wstring& VSName, const std::wstring& PSName,
		std::vector<D3D12_GRAPHICS_PIPELINE_STATE_DESC*>* gpsds,
		const std::wstring& psoName,
		unsigned int FLAG_THREAD);
	~QuadTask();

	void SetQuadComponents(std::vector<component::GUI2DComponent*>* quadComponents);

	void Execute();

private:
	std::vector<component::GUI2DComponent*> m_QuadComponents;

	std::map<E_DEPTH_LEVEL, std::vector<QuadManager*>> m_QuadManagers;
	void draw(ID3D12GraphicsCommandList5* commandList, E_DEPTH_LEVEL type);
};

#endif