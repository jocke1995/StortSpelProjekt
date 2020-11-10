#include "../../Headers/structs.h"

struct srv
{
	float3 pos;
	float size;
};

StructuredBuffer<float4> pos3size[] : register(t0);
RWStructuredBuffer<float4x4> uav[] : register(u0);

//ConstantBuffer<DescriptorHeapIndices> dhIndices : register(b2, space4);

//static const int g_BlurRadius = 4;
//static const int g_NumThreads = 256;
//groupshared float4 g_SharedMem[g_NumThreads + 2 * g_BlurRadius];

[numthreads(5, 1, 1)]
void CS_main(uint3 dispatchThreadID : SV_DispatchThreadID, int3 groupThreadID : SV_GroupThreadID)
{
	float4x4 o = { 1, 0, 1, 0,
					1, 0, 1, 0,
					1, 0, 1, 0,
					1, 0, 1, 0 };

	float4x4 outMat = {
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
	};

	// Write to uav
	uav[0][0] = outMat;
}