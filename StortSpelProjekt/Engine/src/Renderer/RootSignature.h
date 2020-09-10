#ifndef ROOTSIGNATURE_H
#define ROOTSIGNATURE_H

// DX12 Forward Declarations
struct ID3D12RootSignature;
struct ID3D10Blob;
typedef ID3D10Blob ID3DBlob;

enum RS
{
	dtCBV,
	dtSRV,
	CB_PER_OBJECT_CONSTANTS,
	CB_PER_OBJECT_CBV,
	CB_PER_FRAME,
	CB_PER_SCENE,
	SRV1,
	UAV1,
	NUM_PARAMS
};

class RootSignature
{
public:
	RootSignature(ID3D12Device5* device);
	virtual ~RootSignature();

	ID3D12RootSignature* GetRootSig() const;
	ID3DBlob* GetBlob() const;
private:
	// Rootsignature
	ID3D12RootSignature* m_pRootSig = nullptr;
	ID3DBlob* m_pBlob = nullptr;
	void createRootSignatureStructure();
};

#endif