#ifndef COPYTASK_H
#define COPYTASK_H

#include "DX12Task.h"

class Resource;

class CopyTask : public DX12Task
{
public:
	CopyTask(ID3D12Device5* device, COMMAND_INTERFACE_TYPE interfaceType, unsigned int FLAG_THREAD);
	virtual ~CopyTask();

	// tuple(Upload, Default, Data)
	void Submit(std::tuple<Resource*, Resource*, const void*>* Upload_Default_Data);

	virtual void Clear() = 0;

protected:
	std::vector<std::tuple<Resource*, Resource*, const void*>> m_UploadDefaultData;

	void copyResource(
		ID3D12GraphicsCommandList5* commandList,
		Resource* uploadResource, Resource* defaultResource,
		const void* data);
};
#endif
