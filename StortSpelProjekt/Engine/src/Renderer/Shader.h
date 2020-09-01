#ifndef SHADER_H
#define SHADER_H

class Shader
{
public:
	Shader(LPCTSTR path, ShaderType type);
	~Shader();

	ID3DBlob* GetBlob() const;

private:
	ID3DBlob* blob;
	ShaderType type;
	LPCTSTR path;	// Ex: vertexShader1

	void CompileShader();
};

#endif