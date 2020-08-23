#include "PipelineState.h"

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
		return AssetLoader::Get()->LoadShader(fileName, type);
	}
	else if (type == ShaderType::PS)
	{
		return AssetLoader::Get()->LoadShader(fileName, type);
	}
	else if (type == ShaderType::CS)
	{
		return AssetLoader::Get()->LoadShader(fileName, type);
	}
	return nullptr;
}
