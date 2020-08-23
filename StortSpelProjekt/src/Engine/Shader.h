#ifndef SHADER_H
#define SHADER_H

enum class ShaderType 
{ 
	VS = 0, 
	PS = 1, 
	CS = 2,
	UNSPECIFIED = 3
};

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