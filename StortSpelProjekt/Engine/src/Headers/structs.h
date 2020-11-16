#ifndef STRUCTS_H
#define STRUCTS_H

// Light defines
#define MAX_DIR_LIGHTS   10
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS  10

// Animation defines
#define MAX_BONES_PER_VERTEX 10
#define MAX_ANIMATION_MATRICES 100

// This struct can be used to send specific indices as a root constant to the GPU.
// Example usage is when the indices for pp-effects are sent to gpu.
struct DescriptorHeapIndices
{
	unsigned int index0;
	unsigned int index1;
	unsigned int index2;
	unsigned int index3;
};

// Indicies of where the descriptors are stored in the descriptorHeap
struct SlotInfo
{
	unsigned int vertexDataIndex;
	// TextureIndices
	unsigned int textureAlbedo;
	unsigned int textureRoughness;
	unsigned int textureMetallic;
	unsigned int textureNormal;
	unsigned int textureEmissive;
	unsigned int textureOpacity;

	unsigned int pad[1];
};

struct ANIMATION_MATRICES_STRUCT
{
	float4x4 matrices[MAX_ANIMATION_MATRICES];
};

struct MaterialAttributes
{
	// shininess
	float shininess;
	float3 pad1;

	// These colors will be used with "addition" in the shaders
	float4 ambientAdd;
	float4 diffuseAdd;
	float4 specularAdd;
	
	// These colors will be used with "multiplication" in the shaders.
	// They can be used to tint the colors in different ways
	float4 ambientMul;
	float4 diffuseMul;
	float4 specularMul;

	float2 uvScale;
	float2 pad2;
};

struct CB_PER_OBJECT_STRUCT
{
	float4x4 worldMatrix;
	float4x4 WVP;
	SlotInfo info;
};

struct CB_PER_FRAME_STRUCT
{
	float3 camPos;
	float pad0;
	float3 camRight;
	float pad1;
	float3 camUp;
	float pad2;
	float3 camForward;
	float pad3;


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
	float3 color;
	float pad1;

	float castShadow;
	float3 pad2;
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

struct PARTICLE_DATA
{
	float3 position;
	float size;
	float rotation;
};

#endif