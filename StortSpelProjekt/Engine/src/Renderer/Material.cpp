#include "stdafx.h"
#include "Material.h"

Material::Material(SlotInfo* slotInfo)
{
	this->slotInfo = slotInfo;
	this->materialAttributes = new MaterialAttributes();

	this->materialAttributes->shininess = 100;
	this->materialAttributes->ambientAdd  = float4({ 0.0f, 0.0f, 0.0f, 1.0f });
	this->materialAttributes->diffuseAdd  = float4({ 0.0f, 0.0f, 0.0f, 1.0f });
	this->materialAttributes->specularAdd = float4({ 0.0f, 0.0f, 0.0f, 1.0f });
	this->materialAttributes->ambientMul  = float4({ 1.0f, 1.0f, 1.0f, 1.0f });
	this->materialAttributes->diffuseMul  = float4({ 1.0f, 1.0f, 1.0f, 1.0f });
	this->materialAttributes->specularMul = float4({ 1.0f, 1.0f, 1.0f, 1.0f });
	this->materialAttributes->uvScale	  = float2({ 1.0f, 1.0f });
}

Material::Material(const Material* other, SlotInfo* slotInfo)
{
	this->slotInfo = slotInfo;

	this->materialAttributes = new MaterialAttributes();
	this->materialAttributes->shininess   = other->materialAttributes->shininess;
	this->materialAttributes->ambientAdd  = other->materialAttributes->ambientAdd;
	this->materialAttributes->diffuseAdd  = other->materialAttributes->diffuseAdd;
	this->materialAttributes->specularAdd = other->materialAttributes->specularAdd;
	this->materialAttributes->ambientMul  = other->materialAttributes->ambientMul;
	this->materialAttributes->diffuseMul  = other->materialAttributes->diffuseMul;
	this->materialAttributes->specularMul = other->materialAttributes->specularMul;
	this->materialAttributes->uvScale	  = other->materialAttributes->uvScale;
}

Material::~Material()
{
	delete this->materialAttributes;
}

void Material::SetCBV(ConstantBufferView* cbv)
{
	this->cbv = cbv;
}

void Material::SetTexture(TEXTURE_TYPE textureType, Texture* texture)
{
	this->textures[textureType] = texture;
	
	switch (textureType)
	{
	case TEXTURE_TYPE::AMBIENT:
		this->slotInfo->textureAmbient = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::DIFFUSE:
		this->slotInfo->textureDiffuse = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::SPECULAR:
		this->slotInfo->textureSpecular = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::NORMAL:
		this->slotInfo->textureNormal = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::EMISSIVE:
		this->slotInfo->textureEmissive = texture->GetDescriptorHeapIndex();
		break;
	}
}

void Material::SetShininess(float shininess)
{
	this->materialAttributes->shininess = shininess;
}

void Material::SetColorAdd(COLOR_TYPE type, float4 color)
{
	switch (type)
	{
	case COLOR_TYPE::LIGHT_AMBIENT:
		this->materialAttributes->ambientAdd = color;
		break;
	case COLOR_TYPE::LIGHT_DIFFUSE:
		this->materialAttributes->diffuseAdd = color;
		break;
	case COLOR_TYPE::LIGHT_SPECULAR:
		this->materialAttributes->specularAdd = color;
		break;
	}
}

void Material::SetColorMul(COLOR_TYPE type, float4 color)
{
	switch (type)
	{
	case COLOR_TYPE::LIGHT_AMBIENT:
		this->materialAttributes->ambientMul = color;
		break;
	case COLOR_TYPE::LIGHT_DIFFUSE:
		this->materialAttributes->diffuseMul = color;
		break;
	case COLOR_TYPE::LIGHT_SPECULAR:
		this->materialAttributes->specularMul = color;
		break;
	}
}

void Material::SetUVScale(float u, float v)
{
	this->materialAttributes->uvScale = {u, v};
}

Texture* Material::GetTexture(TEXTURE_TYPE textureType)
{
	return this->textures.at(textureType);
}

MaterialAttributes* Material::GetMaterialAttributes() const
{
	return this->materialAttributes;
}

const ConstantBufferView* const Material::GetConstantBufferView() const
{
	return this->cbv;
}
