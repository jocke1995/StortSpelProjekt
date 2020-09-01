#include "stdafx.h"
#include "PipelineState.h"

#include "Shader.h"
#include "RootSignature.h"
#include "../Misc/AssetLoader.h"

PipelineState::PipelineState(LPCTSTR psoName)
{
	this->psoName = psoName;
}

PipelineState::~PipelineState()
{
	SAFE_RELEASE(&this->PSO);
}

ID3D12PipelineState* PipelineState::GetPSO() const
{
	return this->PSO;
}

Shader* PipelineState::CreateShader(LPCTSTR fileName, ShaderType type)
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
