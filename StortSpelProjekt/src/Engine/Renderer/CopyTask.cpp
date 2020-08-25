#include "CopyTask.h"
#include "Resource.h"

CopyTask::CopyTask(ID3D12Device5* device)
	:DX12Task(device, COMMAND_INTERFACE_TYPE::COPY_TYPE)
{

}

CopyTask::~CopyTask()
{
}

void CopyTask::Submit(std::pair<void*, ConstantBufferView*>* data_CBV)
{
	this->data_CBVs.push_back(*data_CBV);
}

void CopyTask::Clear()
{
	this->data_CBVs.clear();
}
