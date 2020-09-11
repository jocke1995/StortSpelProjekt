
Texture2D<float4> brightTarget : register(t1, space1);
RWTexture2D<float4> blurTarget : register(u0, space0);

[numthreads(256, 1, 1)]
void CS_main(uint3 DTid : SV_DispatchThreadID)
{
	
	//blurTarget[0].rgba.r = 0.2f;
}