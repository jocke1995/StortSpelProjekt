#ifndef DX12TASK_H
#define DX12TASK_H

#include "../Misc/Task.h"
#include "CommandInterface.h"

class Resource;

// These renderTasks will execute on "all objects"
enum RENDER_TASK_TYPE
{
	FORWARD_RENDER,
	BLEND,
	SHADOW,
	NR_OF_RENDERTASKS
};

enum COMPUTE_TASK_TYPE
{
	NR_OF_COMPUTETASKS
};

enum COPY_TASK_TYPE
{
	COPY_PER_FRAME,
	NR_OF_COPYTASKS
};

class DX12Task : public Task
{
public:
	DX12Task(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType);
	~DX12Task();

	void SetBackBufferIndex(int backBufferIndex);
	void SetCommandInterfaceIndex(int index);

	ID3D12GraphicsCommandList5* GetCommandList(unsigned int index) const;
protected:

	CommandInterface* commandInterface = nullptr;
	int backBufferIndex = -1;
	int commandInterfaceIndex = -1;
};

#endif