#include "stdafx.h"
#include "GraphicsState.h"

#include "RootSignature.h"
#include "Shader.h"

GraphicsState::GraphicsState(ID3D12Device5* device, RootSignature* rootSignature, LPCWSTR VSName, LPCWSTR PSName, D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpsd, LPCTSTR psoName)
	:PipelineState(psoName)
{
	// Set the rootSignature in the pipeline state object descriptor
	this->m_pGPSD = gpsd;

	this->m_pGPSD->pRootSignature = rootSignature->GetRootSig();

	this->m_pVS = this->createShader(VSName, ShaderType::VS);
	this->m_pPS = this->createShader(PSName, ShaderType::PS);

	ID3DBlob* vsBlob = this->m_pVS->GetBlob();
	ID3DBlob* psBlob = this->m_pPS->GetBlob();

	this->m_pGPSD->VS.pShaderBytecode = vsBlob->GetBufferPointer();
	this->m_pGPSD->VS.BytecodeLength = vsBlob->GetBufferSize();
	this->m_pGPSD->PS.pShaderBytecode = psBlob->GetBufferPointer();
	this->m_pGPSD->PS.BytecodeLength = psBlob->GetBufferSize();

	// Create pipelineStateObject
	HRESULT hr = device->CreateGraphicsPipelineState(this->m_pGPSD, IID_PPV_ARGS(&this->m_pPSO));

	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create %S\n", this->m_PsoName);
	}
	this->m_pPSO->SetName(this->m_PsoName);
}

GraphicsState::~GraphicsState()
{
}

const D3D12_GRAPHICS_PIPELINE_STATE_DESC* GraphicsState::GetGpsd() const
{
	return this->m_pGPSD;
}

Shader* GraphicsState::GetShader(ShaderType type) const
{
	if (type == ShaderType::VS)
	{
		return this->m_pVS;
	}
	else if (type == ShaderType::PS)
	{
		return this->m_pPS;
	}
	
	Log::PrintSeverity(Log::Severity::CRITICAL, "There is no ComputeShader in \'%S\'\n", this->m_PsoName);
	return nullptr;
}
