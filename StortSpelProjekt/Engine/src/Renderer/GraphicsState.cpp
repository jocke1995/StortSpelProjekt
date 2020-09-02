#include "stdafx.h"
#include "GraphicsState.h"

#include "RootSignature.h"
#include "Shader.h"

GraphicsState::GraphicsState(ID3D12Device5* device, RootSignature* rootSignature, LPCWSTR VSName, LPCWSTR PSName, D3D12_GRAPHICS_PIPELINE_STATE_DESC* gpsd, LPCTSTR psoName)
	:PipelineState(psoName)
{
	// Set the rootSignature in the pipeline state object descriptor
	m_pGPSD = gpsd;

	m_pGPSD->pRootSignature = rootSignature->GetRootSig();

	m_pVS = createShader(VSName, ShaderType::VS);
	m_pPS = createShader(PSName, ShaderType::PS);

	ID3DBlob* vsBlob = m_pVS->GetBlob();
	ID3DBlob* psBlob = m_pPS->GetBlob();

	m_pGPSD->VS.pShaderBytecode = vsBlob->GetBufferPointer();
	m_pGPSD->VS.BytecodeLength = vsBlob->GetBufferSize();
	m_pGPSD->PS.pShaderBytecode = psBlob->GetBufferPointer();
	m_pGPSD->PS.BytecodeLength = psBlob->GetBufferSize();

	// Create pipelineStateObject
	HRESULT hr = device->CreateGraphicsPipelineState(m_pGPSD, IID_PPV_ARGS(&m_pPSO));

	if (FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create %S\n", m_PsoName);
	}
	m_pPSO->SetName(m_PsoName);
}

GraphicsState::~GraphicsState()
{
}

const D3D12_GRAPHICS_PIPELINE_STATE_DESC* GraphicsState::GetGpsd() const
{
	return m_pGPSD;
}

Shader* GraphicsState::GetShader(ShaderType type) const
{
	if (type == ShaderType::VS)
	{
		return m_pVS;
	}
	else if (type == ShaderType::PS)
	{
		return m_pPS;
	}
	
	Log::PrintSeverity(Log::Severity::CRITICAL, "There is no ComputeShader in \'%S\'\n", m_PsoName);
	return nullptr;
}
