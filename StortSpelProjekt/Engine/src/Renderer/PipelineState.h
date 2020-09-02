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
	virtual ~PipelineState();

	ID3D12PipelineState* GetPSO() const;
	virtual Shader* GetShader(ShaderType type) const = 0;

protected:
	ID3D12PipelineState* m_pPSO = nullptr;
	LPCTSTR m_PsoName;

	Shader* createShader(LPCTSTR fileName, ShaderType type);
};

#endif