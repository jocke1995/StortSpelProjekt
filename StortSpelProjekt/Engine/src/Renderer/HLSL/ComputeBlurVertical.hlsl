#include "../../Headers/structs.h"

Texture2D<float4> textures[]   : register (t0);
RWTexture2D<float4> textureToBlur[] : register(u0);

ConstantBuffer<DescriptorHeapIndices> dhIndices : register(b2, space4);

[numthreads(1, 256, 1)]
void CS_main(uint3 DTid : SV_DispatchThreadID)
{
	unsigned int readIndex = dhIndices.index2;
	unsigned int writeIndex = dhIndices.index3;

	textureToBlur[writeIndex][DTid.xy] = textures[readIndex][DTid.xy];
}