#ifndef COPYTASK_H
#define COPYTASK_H

#include "DX12Task.h"

#include "../ConstantBufferView.h"

class CopyTask : public DX12Task
{
public:
	CopyTask(ID3D12Device5* device);
	virtual ~CopyTask();

	// Data alongside its constantBuffer
	void Submit(std::pair<const void*, ConstantBufferView*>* data_CBV);

	virtual void Clear() = 0;

protected:
	std::vector<std::pair<const void*, ConstantBufferView*>> m_Data_CBVs;

	void copyResource(
		ID3D12GraphicsCommandList5* commandList,
		Resource* uploadResource, Resource* defaultResource,
		const void* data);
};
#endif
