#ifndef SHADER_H
#define SHADER_H

class Shader
{
public:
	Shader(LPCTSTR path, ShaderType type);
	virtual ~Shader();

	ID3DBlob* GetBlob() const;

private:
	ID3DBlob* m_pBlob;
	ShaderType m_Type;
	LPCTSTR m_Path;	// Ex: vertexShader1

	void compileShader();
};

#endif