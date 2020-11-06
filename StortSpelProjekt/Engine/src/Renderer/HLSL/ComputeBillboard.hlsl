#include "../../Headers/structs.h"

struct srv
{
	float3 pos;
	float size;
};

StructuredBuffer<srv> srvIn[]   : register (t0);
RWStructuredBuffer<float4x4> uavOut[] : register(u0);

//ConstantBuffer<DescriptorHeapIndices> dhIndices : register(b2, space4);

//static const int g_BlurRadius = 4;
//static const int g_NumThreads = 256;
//groupshared float4 g_SharedMem[g_NumThreads + 2 * g_BlurRadius];

[numthreads(1, 1, 1)]
void CS_main(uint3 dispatchThreadID : SV_DispatchThreadID, int3 groupThreadID : SV_GroupThreadID)
{
	float4x4 o = { 1, 0, 1, 0,
					1, 0, 1, 0,
					1, 0, 1, 0,
					1, 0, 1, 0 };

	uavOut[0][0] = o;
}