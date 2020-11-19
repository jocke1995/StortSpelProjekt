#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

// for std::wString
#include <string>

class ConstantBuffer;
class DescriptorHeap;

struct ID3D12Device5;

static unsigned int s_ProgressBarCounter = 0;
class ProgressBar
{
public:
    ProgressBar(
		ID3D12Device5* device,
		unsigned int size,
		std::wstring resourceName,
		DescriptorHeap* dh_CBV_SRV_UAV);
    virtual ~ProgressBar();

	ConstantBuffer* GetConstantBuffer() const;
private:
    ConstantBuffer* m_pConstantBuffer = nullptr;

	unsigned int m_Id = 0;
};

#endif
