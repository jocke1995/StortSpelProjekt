#ifndef PIPELINESTATE_H
#define PIPELINESTATE_H

class Shader;
class RootSignature;
class AssetLoader;

// DX12 Forward Declarations
struct ID3D12PipelineState;

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