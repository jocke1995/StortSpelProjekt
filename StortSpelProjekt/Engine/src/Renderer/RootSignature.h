#ifndef ROOTSIGNATURE_H
#define ROOTSIGNATURE_H


enum RS
{
	dtCBV,
	dtSRV,
	CB_PER_OBJECT_CONSTANTS,
	CB_PER_FRAME,
	CB_PER_SCENE,
	NUM_PARAMS
};

class RootSignature
{
public:
	RootSignature(ID3D12Device5* device);
	~RootSignature();

	ID3D12RootSignature* GetRootSig() const;
	ID3DBlob* GetBlob() const;
private:
	// Rootsignature
	ID3D12RootSignature* rootSig;
	ID3DBlob* sBlob;
	void CreateRootSignatureStructure();
};

#endif