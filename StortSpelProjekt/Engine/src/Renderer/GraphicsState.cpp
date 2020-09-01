#include "stdafx.h"
#include "GraphicsState.h"

#include "RootSignature.h"
#include "Shader.h"

GraphicsState::GraphicsState(ID3D12Device5* device, RootSignature* rootSignature, LPCWSTR VSName, LPCWSTR PSName, D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpsd, LPCTSTR psoName)
	:PipelineState(psoName)
{
	// Set the rootSignature in the pipeline state object descriptor
	this->gpsd = gpsd;

	this->gpsd->pRootSignature = rootSignature->GetRootSig();

	this->VS = this->CreateShader(VSName, ShaderType::VS);
	this->PS = this->CreateShader(PSName, ShaderType::PS);

	ID3DBlob* vsBlob = this->VS->GetBlob();
	ID3DBlob* psBlob = this->PS->GetBlob();

	this->gpsd->VS.pShaderBytecode = vsBlob->GetBufferPointer();
	this->gpsd->VS.BytecodeLength = vsBlob->GetBufferSize();
	this->gpsd->PS.pShaderBytecode = psBlob->GetBufferPointer();
	this->gpsd->PS.BytecodeLength = psBlob->GetBufferSize();

	// Create pipelineStateObject
	HRESULT hr = device->CreateGraphicsPipelineState(this->gpsd, IID_PPV_ARGS(&this->PSO));

	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create %S\n", this->psoName);
	}
	this->PSO->SetName(this->psoName);
}

GraphicsState::~GraphicsState()
{
}

const D3D12_GRAPHICS_PIPELINE_STATE_DESC* GraphicsState::GetGpsd() const
{
	return this->gpsd;
}

Shader* GraphicsState::GetShader(ShaderType type) const
{
	if (type == ShaderType::VS)
	{
		return this->VS;
	}
	else if (type == ShaderType::PS)
	{
		return this->PS;
	}
	
	Log::PrintSeverity(Log::Severity::CRITICAL, "There is no ComputeShader in \'%S\'\n", this->psoName);
	return nullptr;
}
