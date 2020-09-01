#ifndef COMPUTESTATE_H
#define COMPUTESTATE_H

#include "PipelineState.h"
#include "Core.h"

class Shader;

class ComputeState : public PipelineState
{
public:
	ComputeState(	ID3D12Device5* device, 
					RootSignature* rootSignature, 
					LPCWSTR CSName,
					LPCTSTR psoName);

	virtual ~ComputeState();

	const D3D12_COMPUTE_PIPELINE_STATE_DESC* GetCpsd() const;
	Shader* GetShader(ShaderType type = ShaderType::CS) const;
private:
	Shader* CS = nullptr;
	D3D12_COMPUTE_PIPELINE_STATE_DESC cpsd = {};
};

#endif 