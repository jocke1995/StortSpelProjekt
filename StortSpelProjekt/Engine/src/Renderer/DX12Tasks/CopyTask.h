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
	void Submit(std::tuple<Resource*, Resource*, const void*>* data_CBV);

	virtual void Clear() = 0;

protected:
	std::vector<std::tuple<Resource*, Resource*, const void*>> m_Upload_Default_Data;

	void copyResource(
		ID3D12GraphicsCommandList5* commandList,
		Resource* uploadResource, Resource* defaultResource,
		const void* data);
};
#endif
