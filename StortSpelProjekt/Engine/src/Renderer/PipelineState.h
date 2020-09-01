#ifndef PIPELINESTATE_H
#define PIPELINESTATE_H

#include "Shader.h"
#include "RootSignature.h"
#include "../Misc/AssetLoader.h"

class PipelineState
{
public:
	PipelineState(LPCTSTR psoName);
	~PipelineState();

	ID3D12PipelineState* GetPSO() const;
	virtual Shader* GetShader(ShaderType type) const = 0;

protected:
	Shader* CreateShader(LPCTSTR fileName, ShaderType type);
	ID3D12PipelineState* PSO = nullptr;
	LPCTSTR psoName;
};

#endif