#include "Shader.h"

Shader::Shader(LPCTSTR path, ShaderType type)
{
	this->path = path;
	this->type = type;

	this->CompileShader();
}

Shader::~Shader()
{
	SAFE_RELEASE(&this->blob);
}

ID3DBlob* Shader::GetBlob() const
{
	return this->blob;
}

void Shader::CompileShader()
{
	std::string entryPoint;
	std::string shaderModelTarget;

	if (type == ShaderType::VS)
	{
		entryPoint = "VS_main";
		shaderModelTarget = "vs_5_1";
	}
	else if (type == ShaderType::PS)
	{
		entryPoint = "PS_main";
		shaderModelTarget = "ps_5_1";
	}
	else if (type == ShaderType::CS)
	{
		entryPoint = "CS_main";
		shaderModelTarget = "cs_5_1";
	}
	// Add geometry shader..? naah

	// shadelModelTarget = fx_5_0

	ID3DBlob* errorMessages = nullptr;


	unsigned int flags = D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = D3DCompileFromFile(
		this->path, // filePath + filename
		nullptr,		// optional macros
		D3D_COMPILE_STANDARD_FILE_INCLUDE,		// optional include files
		entryPoint.c_str(),		// entry point
		shaderModelTarget.c_str(),		// shader model (target)
		flags,	// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&this->blob,	// double pointer to ID3DBlob		
		&errorMessages			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);

	if (blob == nullptr)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "blob is nullptr when loading shader with path: %S\n", this->path);
	}

	if (FAILED(hr) && errorMessages)
	{
		const char* errorMsg = (const char*)errorMessages->GetBufferPointer();

		Log::PrintSeverity(Log::Severity::CRITICAL, "%s\n", errorMsg);
	}
}
