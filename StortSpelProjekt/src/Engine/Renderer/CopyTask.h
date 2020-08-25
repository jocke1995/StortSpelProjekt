#ifndef COPYTASK_H
#define COPYTASK_H

#include "DX12Task.h"

#include "ConstantBufferView.h"

class CopyTask : public DX12Task
{
public:
	CopyTask(ID3D12Device5* device);
	virtual ~CopyTask();

	void Submit(std::pair<void*, ConstantBufferView*>* data_CBV);

	void Clear();
protected:
	std::vector<std::pair<void*, ConstantBufferView*>> data_CBVs;
};
#endif
