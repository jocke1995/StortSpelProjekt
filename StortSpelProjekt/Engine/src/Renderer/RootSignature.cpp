#include "stdafx.h"
#include "RootSignature.h"

#include "d3dx12.h"

RootSignature::RootSignature(ID3D12Device5* device)
{
	createRootSignatureStructure();

	HRESULT hr = device->CreateRootSignature(
		0,
		m_pBlob->GetBufferPointer(),
		m_pBlob->GetBufferSize(),
		IID_PPV_ARGS(&m_pRootSig));

	if(FAILED(hr))
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to create RootSignature\n");
	}
}

RootSignature::~RootSignature()
{
	SAFE_RELEASE(&m_pRootSig);
	SAFE_RELEASE(&m_pBlob);
}

ID3D12RootSignature* RootSignature::GetRootSig() const
{
	return m_pRootSig;
}

ID3DBlob* RootSignature::GetBlob() const
{
	return m_pBlob;
}

void RootSignature::createRootSignatureStructure()
{
	// DescriptorTable for CBV's (bindless)
	D3D12_DESCRIPTOR_RANGE dtRangesCBV[3]{};
	dtRangesCBV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	dtRangesCBV[0].NumDescriptors = -1;
	dtRangesCBV[0].BaseShaderRegister = 0;	// b0
	dtRangesCBV[0].RegisterSpace = 0;		// space0
	dtRangesCBV[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	dtRangesCBV[1].NumDescriptors = -1;
	dtRangesCBV[1].BaseShaderRegister = 0;	// b0
	dtRangesCBV[1].RegisterSpace = 1;		// space1
	dtRangesCBV[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	dtRangesCBV[2].NumDescriptors = -1;
	dtRangesCBV[2].BaseShaderRegister = 0;	// b0
	dtRangesCBV[2].RegisterSpace = 2;		// space2
	D3D12_ROOT_DESCRIPTOR_TABLE dtCBV = {};
	dtCBV.NumDescriptorRanges = ARRAYSIZE(dtRangesCBV);
	dtCBV.pDescriptorRanges = dtRangesCBV;

	// DescriptorTable for SRV's (bindless)
	D3D12_DESCRIPTOR_RANGE dtRangesSRV[1]{};
	dtRangesSRV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	dtRangesSRV[0].NumDescriptors = -1; // Bindless
	dtRangesSRV[0].BaseShaderRegister = 0;	// t0
	dtRangesSRV[0].RegisterSpace = 0;

	D3D12_ROOT_DESCRIPTOR_TABLE dtSRV = {};
	dtSRV.NumDescriptorRanges = ARRAYSIZE(dtRangesSRV);
	dtSRV.pDescriptorRanges = dtRangesSRV;

	// DescriptorTable for UAV's (bindless)
	D3D12_DESCRIPTOR_RANGE dtRangesUAV[1]{};
	dtRangesUAV[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	dtRangesUAV[0].NumDescriptors = -1; // Bindless
	dtRangesUAV[0].BaseShaderRegister = 0;	// u0
	dtRangesUAV[0].RegisterSpace = 0;

	D3D12_ROOT_DESCRIPTOR_TABLE dtUAV = {};
	dtUAV.NumDescriptorRanges = ARRAYSIZE(dtRangesUAV);
	dtUAV.pDescriptorRanges = dtRangesUAV;

	D3D12_ROOT_PARAMETER rootParam[RS::NUM_PARAMS]{};

	rootParam[RS::dtCBV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[RS::dtCBV].DescriptorTable = dtCBV;
	rootParam[RS::dtCBV].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParam[RS::dtSRV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[RS::dtSRV].DescriptorTable = dtSRV;
	rootParam[RS::dtSRV].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParam[RS::dtUAV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[RS::dtUAV].DescriptorTable = dtUAV;
	rootParam[RS::dtUAV].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParam[RS::CB_PER_OBJECT_CONSTANTS].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParam[RS::CB_PER_OBJECT_CONSTANTS].Constants.ShaderRegister = 1; // b1
	rootParam[RS::CB_PER_OBJECT_CONSTANTS].Constants.RegisterSpace = 3; // space3
	rootParam[RS::CB_PER_OBJECT_CONSTANTS].Constants.Num32BitValues = sizeof(CB_PER_OBJECT_STRUCT) / sizeof(UINT);
	rootParam[RS::CB_PER_OBJECT_CONSTANTS].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParam[RS::CB_INDICES_CONSTANTS].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParam[RS::CB_INDICES_CONSTANTS].Constants.ShaderRegister = 2; // b2
	rootParam[RS::CB_INDICES_CONSTANTS].Constants.RegisterSpace = 4; // space4
	rootParam[RS::CB_INDICES_CONSTANTS].Constants.Num32BitValues = sizeof(DescriptorHeapIndices) / sizeof(UINT);
	rootParam[RS::CB_INDICES_CONSTANTS].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParam[RS::CB_PER_OBJECT_CBV].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParam[RS::CB_PER_OBJECT_CBV].Constants.ShaderRegister = 2; // b2
	rootParam[RS::CB_PER_OBJECT_CBV].Constants.RegisterSpace = 3; // space3
	rootParam[RS::CB_PER_OBJECT_CBV].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParam[RS::CB_PER_FRAME].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParam[RS::CB_PER_FRAME].Constants.ShaderRegister = 3; // b3
	rootParam[RS::CB_PER_FRAME].Constants.RegisterSpace = 3; // space3
	rootParam[RS::CB_PER_FRAME].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParam[RS::CB_PER_SCENE].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParam[RS::CB_PER_SCENE].Descriptor.ShaderRegister = 4;	// b4
	rootParam[RS::CB_PER_SCENE].Descriptor.RegisterSpace = 3;	// space3
	rootParam[RS::CB_PER_SCENE].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	const unsigned int numStaticSamplers = 6;
	D3D12_ROOT_SIGNATURE_DESC rsDesc;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	rsDesc.NumParameters = ARRAYSIZE(rootParam);
	rsDesc.pParameters = rootParam;
	rsDesc.NumStaticSamplers = numStaticSamplers;

	D3D12_STATIC_SAMPLER_DESC ssd[numStaticSamplers] = {};

	// Anisotropic Wrap
	for (unsigned int i = 1; i < 5; i++)
	{
		ssd[i - 1].ShaderRegister = i - 1;
		ssd[i - 1].Filter = D3D12_FILTER_ANISOTROPIC;
		ssd[i - 1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		ssd[i - 1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		ssd[i - 1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		ssd[i - 1].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		ssd[i - 1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		ssd[i - 1].MinLOD = 0;
		ssd[i - 1].MaxLOD = D3D12_FLOAT32_MAX;
		ssd[i - 1].MaxAnisotropy = 2 * i;
	}

	ssd[4].ShaderRegister = 4;
	ssd[4].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	ssd[4].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	ssd[4].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	ssd[4].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	ssd[4].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	ssd[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	ssd[4].MinLOD = 0;
	ssd[4].MaxLOD = D3D12_FLOAT32_MAX;
	ssd[4].MipLODBias = 0.0f;
	ssd[4].MaxAnisotropy = 1;
	ssd[4].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;

	ssd[5].ShaderRegister = 5;
	ssd[5].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	ssd[5].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	ssd[5].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	ssd[5].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	ssd[5].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	ssd[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	ssd[5].MinLOD = 0;
	ssd[5].MaxLOD = D3D12_FLOAT32_MAX;
	ssd[5].MipLODBias = 0.0f;

	rsDesc.pStaticSamplers = ssd;


	ID3DBlob* errorMessages = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&m_pBlob,
		&errorMessages);

	if (FAILED(hr) && errorMessages)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Serialize RootSignature\n");

		const char* errorMsg = static_cast<const char*>(errorMessages->GetBufferPointer());
		Log::PrintSeverity(Log::Severity::CRITICAL, "%s\n", errorMsg);
	}
}
