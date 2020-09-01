#include "stdafx.h"
#include "Material.h"

#include "Texture.h"
#include "ConstantBufferView.h"

Material::Material(SlotInfo* slotInfo)
{
	this->m_pSlotInfo = slotInfo;
	this->m_pMaterialAttributes = new MaterialAttributes();

	this->m_pMaterialAttributes->shininess = 100;
	this->m_pMaterialAttributes->ambientAdd  = float4({ 0.0f, 0.0f, 0.0f, 1.0f });
	this->m_pMaterialAttributes->diffuseAdd  = float4({ 0.0f, 0.0f, 0.0f, 1.0f });
	this->m_pMaterialAttributes->specularAdd = float4({ 0.0f, 0.0f, 0.0f, 1.0f });
	this->m_pMaterialAttributes->ambientMul  = float4({ 1.0f, 1.0f, 1.0f, 1.0f });
	this->m_pMaterialAttributes->diffuseMul  = float4({ 1.0f, 1.0f, 1.0f, 1.0f });
	this->m_pMaterialAttributes->specularMul = float4({ 1.0f, 1.0f, 1.0f, 1.0f });
	this->m_pMaterialAttributes->uvScale	  = float2({ 1.0f, 1.0f });
}

Material::Material(const Material* other, SlotInfo* slotInfo)
{
	this->m_pSlotInfo = slotInfo;

	this->m_pMaterialAttributes = new MaterialAttributes();
	this->m_pMaterialAttributes->shininess   = other->m_pMaterialAttributes->shininess;
	this->m_pMaterialAttributes->ambientAdd  = other->m_pMaterialAttributes->ambientAdd;
	this->m_pMaterialAttributes->diffuseAdd  = other->m_pMaterialAttributes->diffuseAdd;
	this->m_pMaterialAttributes->specularAdd = other->m_pMaterialAttributes->specularAdd;
	this->m_pMaterialAttributes->ambientMul  = other->m_pMaterialAttributes->ambientMul;
	this->m_pMaterialAttributes->diffuseMul  = other->m_pMaterialAttributes->diffuseMul;
	this->m_pMaterialAttributes->specularMul = other->m_pMaterialAttributes->specularMul;
	this->m_pMaterialAttributes->uvScale	  = other->m_pMaterialAttributes->uvScale;

	for (unsigned int i = 0; i < TEXTURE_TYPE::NUM_TEXTURE_TYPES; i++)
	{
		TEXTURE_TYPE type = static_cast<TEXTURE_TYPE>(i);
		this->m_Textures[type] = other->m_Textures.at(type);
	}
}

Material::~Material()
{
	delete this->m_pMaterialAttributes;
}

void Material::SetCBV(ConstantBufferView* cbv)
{
	this->m_pCbv = cbv;
}

void Material::SetTexture(TEXTURE_TYPE textureType, Texture* texture)
{
	this->m_Textures[textureType] = texture;
	
	switch (textureType)
	{
	case TEXTURE_TYPE::AMBIENT:
		this->m_pSlotInfo->textureAmbient = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::DIFFUSE:
		this->m_pSlotInfo->textureDiffuse = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::SPECULAR:
		this->m_pSlotInfo->textureSpecular = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::NORMAL:
		this->m_pSlotInfo->textureNormal = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::EMISSIVE:
		this->m_pSlotInfo->textureEmissive = texture->GetDescriptorHeapIndex();
		break;
	}
}

void Material::SetShininess(float shininess)
{
	this->m_pMaterialAttributes->shininess = shininess;
}

void Material::SetColorAdd(COLOR_TYPE type, float4 color)
{
	switch (type)
	{
	case COLOR_TYPE::LIGHT_AMBIENT:
		this->m_pMaterialAttributes->ambientAdd = color;
		break;
	case COLOR_TYPE::LIGHT_DIFFUSE:
		this->m_pMaterialAttributes->diffuseAdd = color;
		break;
	case COLOR_TYPE::LIGHT_SPECULAR:
		this->m_pMaterialAttributes->specularAdd = color;
		break;
	}
}

void Material::SetColorMul(COLOR_TYPE type, float4 color)
{
	switch (type)
	{
	case COLOR_TYPE::LIGHT_AMBIENT:
		this->m_pMaterialAttributes->ambientMul = color;
		break;
	case COLOR_TYPE::LIGHT_DIFFUSE:
		this->m_pMaterialAttributes->diffuseMul = color;
		break;
	case COLOR_TYPE::LIGHT_SPECULAR:
		this->m_pMaterialAttributes->specularMul = color;
		break;
	}
}

void Material::SetUVScale(float u, float v)
{
	this->m_pMaterialAttributes->uvScale = {u, v};
}

Texture* Material::GetTexture(TEXTURE_TYPE textureType)
{
	return this->m_Textures.at(textureType);
}

MaterialAttributes* Material::GetMaterialAttributes() const
{
	return this->m_pMaterialAttributes;
}

const ConstantBufferView* const Material::GetConstantBufferView() const
{
	return this->m_pCbv;
}
