Texture2D textures[] : register(t0);
SamplerState samplerTypeWrap : register (s0);

struct VS_OUT
{
	float4 pos: SV_POSITION;
	float4 color: COLOR;
	float2 texCoord: TEXCOORD;
};

float4 PS_main(VS_OUT input) : SV_TARGET0
{
	float4 textureTmp = textures[3].Sample(samplerTypeWrap, input.texCoord);
	//return float4(input.texCoord.x, input.texCoord.y, 0.0, 1.0);//*/textureTmp;
	//return float4(input.color.rgb, input.color.a * textures[1].Sample(samplerTypeWrap, input.texCoord).a);
	return input.color;
}