#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_DIR_LIGHTS   10
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS  10

// Indicies of where the descriptors are stored in the descriptorHeap
struct SlotInfo
{
	unsigned int vertexDataIndex;
	// TextureIndices
	unsigned int textureAmbient;
	unsigned int textureDiffuse;
	unsigned int textureSpecular;
	unsigned int textureNormal;
	unsigned int textureEmissive;

	unsigned int pad1[2];
};

struct MaterialAttributes
{
	float shininess;
	float3 pad1;
};

struct CB_PER_OBJECT_STRUCT
{
	float4x4 worldMatrix;
	float4x4 WVP;
	MaterialAttributes matAttrib;
	SlotInfo info;
};

struct CB_PER_FRAME_STRUCT
{
	float3 camPos;
	float pad1;
	// deltaTime ..
	// etc ..
};

struct CB_PER_SCENE_STRUCT
{
	float4 dirLightIndices[MAX_DIR_LIGHTS];
	float4 pointLightIndices[MAX_POINT_LIGHTS];
	float4 spotLightIndices[MAX_SPOT_LIGHTS];

	unsigned int Num_Dir_Lights;
	unsigned int Num_Point_Lights;
	unsigned int Num_Spot_Lights;
	unsigned int pad1;
};

struct BaseLight
{
	float4 ambient;
	float4 diffuse;
	float4 specular;

	float castShadow;
	float3 pad1;
};

struct DirectionalLight
{
	float4 direction;
	BaseLight baseLight;

	float4x4 viewProj;

	unsigned int textureShadowMap;	// Index to the shadowMap (srv)
	unsigned int pad1[3];
};

struct PointLight
{
	float4 position;
	float4 attenuation;	// 4byte-constant, 4byte-linear, 4byte-quadratic, 4byte-padding

	BaseLight baseLight;
};

struct SpotLight
{
	float4x4 viewProj;

	float4 position_cutOff;			// position  .x.y.z & cutOff in .w (cutOff = radius)
	float4 direction_outerCutoff;	// direction .x.y.z & outerCutOff in .w
	float4 attenuation;	// 4byte-constant, 4byte-linear, 4byte-quadratic, 4byte-padding
	BaseLight baseLight;

	unsigned int textureShadowMap;	// Index to the shadowMap (srv)
	unsigned int pad1[3];
};

#endif