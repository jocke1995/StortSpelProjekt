#include "stdafx.h"
#include "ComputeState.h"

#include "RootSignature.h"
#include "Shader.h"

ComputeState::ComputeState(ID3D12Device5* device, RootSignature* rootSignature, LPCWSTR CSName, LPCTSTR psoName)
	:PipelineState(psoName)
{
	this->cpsd.pRootSignature = rootSignature->GetRootSig();

	this->CS = this->CreateShader(CSName, ShaderType::CS);

	ID3DBlob* csBlob = this->CS->GetBlob();

	this->cpsd.CS.pShaderBytecode = csBlob->GetBufferPointer();
	this->cpsd.CS.BytecodeLength = csBlob->GetBufferSize();

	// Create pipelineStateObject
	HRESULT hr = device->CreateComputePipelineState(&this->cpsd, IID_PPV_ARGS(&this->PSO));

	this->PSO->SetName(this->psoName);
	if (!SUCCEEDED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create %S\n", this->psoName);
	}
}

ComputeState::~ComputeState()
{
}

const D3D12_COMPUTE_PIPELINE_STATE_DESC* ComputeState::GetCpsd() const
{
	return &this->cpsd;
}

Shader* ComputeState::GetShader(ShaderType type) const
{
	if (type == ShaderType::CS)
	{
		return this->CS;
	}
	else if (type == ShaderType::VS)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "There is no vertexShader in \'%S\'\n", this->psoName);
	}
	else if (type == ShaderType::PS)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "There is no pixelShader in \'%S\'\n", this->psoName);
	}

	return nullptr;
}
