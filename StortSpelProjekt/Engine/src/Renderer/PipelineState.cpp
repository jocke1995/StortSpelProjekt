#include "stdafx.h"
#include "PipelineState.h"

#include "Shader.h"
#include "RootSignature.h"
#include "../Misc/AssetLoader.h"

PipelineState::PipelineState(LPCTSTR psoName)
{
	m_PsoName = psoName;
}

PipelineState::~PipelineState()
{
	SAFE_RELEASE(&m_pPSO);
}

ID3D12PipelineState* PipelineState::GetPSO() const
{
	return m_pPSO;
}

Shader* PipelineState::createShader(LPCTSTR fileName, ShaderType type)
{
	if (type == ShaderType::VS)
	{
		return AssetLoader::Get()->loadShader(fileName, type);
	}
	else if (type == ShaderType::PS)
	{
		return AssetLoader::Get()->loadShader(fileName, type);
	}
	else if (type == ShaderType::CS)
	{
		return AssetLoader::Get()->loadShader(fileName, type);
	}
	return nullptr;
}
