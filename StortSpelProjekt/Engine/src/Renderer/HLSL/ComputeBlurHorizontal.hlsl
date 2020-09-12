Texture2D<float4> textures[]   : register (t0);
RWTexture2D<float4> textureToBlur[] : register(u0);

[numthreads(256, 1, 1)]
void CS_main(uint3 DTid : SV_DispatchThreadID)
{
	textureToBlur[5][DTid.xy] = textures[2][DTid.xy] * 1.5;
}