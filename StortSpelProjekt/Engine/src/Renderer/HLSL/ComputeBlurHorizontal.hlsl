#include "../../Headers/structs.h"

Texture2D<float4> textures[]   : register (t0);
RWTexture2D<float4> textureToBlur[] : register(u0);

ConstantBuffer<DescriptorHeapIndices> dhIndices : register(b2, space3);

static const int g_BlurRadius = 4;
static const int g_NumThreads = 256;
groupshared float4 g_SharedMem[g_NumThreads + 2 * g_BlurRadius];

[numthreads(g_NumThreads, 1, 1)]
void CS_main(uint3 dispatchThreadID : SV_DispatchThreadID, int3 groupThreadID : SV_GroupThreadID)
{
	unsigned int readIndex = dhIndices.index0;
	unsigned int writeIndex = dhIndices.index1;

	float weights[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
	/* -------------------- Clamp out of bound samples -------------------- */
	// left side
	if (groupThreadID.x < g_BlurRadius)
	{
		int x = max(dispatchThreadID.x - g_BlurRadius, 0);
		g_SharedMem[groupThreadID.x] = textures[readIndex][int2(x, dispatchThreadID.y)];
	}
	
	// right side
	if (groupThreadID.x >= g_NumThreads - g_BlurRadius)
	{
		int x = min(dispatchThreadID.x + g_BlurRadius, textures[readIndex].Length.x - 1);
		g_SharedMem[groupThreadID.x + 2 * g_BlurRadius] = textures[readIndex][int2(x, dispatchThreadID.y)];
	}
	/* -------------------- Clamp out of bound samples -------------------- */

	// Fill the middle parts of the sharedMemory
	g_SharedMem[groupThreadID.x + g_BlurRadius] = textures[readIndex][min(dispatchThreadID.xy, textures[readIndex].Length.xy - 1)];

	// Wait for shared memory to be populated before reading from it
	GroupMemoryBarrierWithGroupSync();
	
	// Blur
	// Current fragments contribution
	float4 blurColor = weights[0] * g_SharedMem[groupThreadID.x + g_BlurRadius];

	// Adjacent fragment contributions
	for (int i = 1; i <= g_BlurRadius; i++)
	{
		int left = groupThreadID.x + g_BlurRadius - i;
		int right = groupThreadID.x + g_BlurRadius + i;
		blurColor += weights[i] * g_SharedMem[left];
		blurColor += weights[i] * g_SharedMem[right];
	}

	textureToBlur[writeIndex][dispatchThreadID.xy] = blurColor;
}