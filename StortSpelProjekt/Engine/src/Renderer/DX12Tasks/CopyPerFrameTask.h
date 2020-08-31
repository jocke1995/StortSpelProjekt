#ifndef COPYPERFRAMETASK_H
#define COPYPERFRAMETASK_H

#include "CopyTask.h"

class CopyPerFrameTask : public CopyTask
{
public:
	CopyPerFrameTask(ID3D12Device5* device);
	virtual ~CopyPerFrameTask();

	// The submit is inside CopyTask

	// Removal
	void ClearSpecific(const ConstantBufferView* cbv);
	void Clear();

	void Execute();

private:
	
};

#endif