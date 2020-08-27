#ifndef COPYPERFRAMETASK_H
#define COPYPERFRAMETASK_H

#include "CopyTask.h"
#include "../Headers/stdafx.h"

class CopyPerFrameTask : public CopyTask
{
public:
	CopyPerFrameTask(ID3D12Device5* device);
	virtual ~CopyPerFrameTask();

	// Data to copy
	void Submit(std::pair<void*, ConstantBufferView*>* data_CBV);

	// Removal
	void ClearSpecific(ConstantBufferView* cbv);
	void Clear();

	void Execute();

private:
	std::vector<std::pair<void*, ConstantBufferView*>> data_CBVs;

	void CopyResource(
		ID3D12GraphicsCommandList5* commandList,
		Resource* uploadResource, Resource* defaultResource,
		void* data);
};

#endif