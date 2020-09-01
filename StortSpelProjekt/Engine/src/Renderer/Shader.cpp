#include "stdafx.h"
#include "Shader.h"

Shader::Shader(LPCTSTR path, ShaderType type)
{
	this->m_Path = path;
	this->m_Type = type;

	this->compileShader();
}

Shader::~Shader()
{
	SAFE_RELEASE(&this->m_pBlob);
}

ID3DBlob* Shader::GetBlob() const
{
	return this->m_pBlob;
}

void Shader::compileShader()
{
	std::string entryPoint;
	std::string shaderModelTarget;

	if (m_Type == ShaderType::VS)
	{
		entryPoint = "VS_main";
		shaderModelTarget = "vs_5_1";
	}
	else if (m_Type == ShaderType::PS)
	{
		entryPoint = "PS_main";
		shaderModelTarget = "ps_5_1";
	}
	else if (m_Type == ShaderType::CS)
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
		this->m_Path, // filePath + filename
		nullptr,		// optional macros
		D3D_COMPILE_STANDARD_FILE_INCLUDE,		// optional include files
		entryPoint.c_str(),		// entry point
		shaderModelTarget.c_str(),		// shader model (target)
		flags,	// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&this->m_pBlob,	// double pointer to ID3DBlob		
		&errorMessages			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);

	if (m_pBlob == nullptr)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "blob is nullptr when loading shader with path: %S\n", this->m_Path);
	}

	if (FAILED(hr) && errorMessages)
	{
		const char* errorMsg = (const char*)errorMessages->GetBufferPointer();

		Log::PrintSeverity(Log::Severity::CRITICAL, "%s\n", errorMsg);
	}
}
