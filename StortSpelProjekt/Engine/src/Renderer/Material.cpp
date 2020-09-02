#include "stdafx.h"
#include "Material.h"

#include "Texture.h"
#include "ConstantBufferView.h"

Material::Material(SlotInfo* slotInfo)
{
	m_pSlotInfo = slotInfo;
	m_pMaterialAttributes = new MaterialAttributes();

	m_pMaterialAttributes->shininess = 100;
	m_pMaterialAttributes->ambientAdd  = float4({ 0.0f, 0.0f, 0.0f, 1.0f });
	m_pMaterialAttributes->diffuseAdd  = float4({ 0.0f, 0.0f, 0.0f, 1.0f });
	m_pMaterialAttributes->specularAdd = float4({ 0.0f, 0.0f, 0.0f, 1.0f });
	m_pMaterialAttributes->ambientMul  = float4({ 1.0f, 1.0f, 1.0f, 1.0f });
	m_pMaterialAttributes->diffuseMul  = float4({ 1.0f, 1.0f, 1.0f, 1.0f });
	m_pMaterialAttributes->specularMul = float4({ 1.0f, 1.0f, 1.0f, 1.0f });
	m_pMaterialAttributes->uvScale	  = float2({ 1.0f, 1.0f });
}

Material::Material(const Material* other, SlotInfo* slotInfo)
{
	m_pSlotInfo = slotInfo;

	m_pMaterialAttributes = new MaterialAttributes();
	m_pMaterialAttributes->shininess   = other->m_pMaterialAttributes->shininess;
	m_pMaterialAttributes->ambientAdd  = other->m_pMaterialAttributes->ambientAdd;
	m_pMaterialAttributes->diffuseAdd  = other->m_pMaterialAttributes->diffuseAdd;
	m_pMaterialAttributes->specularAdd = other->m_pMaterialAttributes->specularAdd;
	m_pMaterialAttributes->ambientMul  = other->m_pMaterialAttributes->ambientMul;
	m_pMaterialAttributes->diffuseMul  = other->m_pMaterialAttributes->diffuseMul;
	m_pMaterialAttributes->specularMul = other->m_pMaterialAttributes->specularMul;
	m_pMaterialAttributes->uvScale	  = other->m_pMaterialAttributes->uvScale;

	for (unsigned int i = 0; i < TEXTURE_TYPE::NUM_TEXTURE_TYPES; i++)
	{
		TEXTURE_TYPE type = static_cast<TEXTURE_TYPE>(i);
		m_Textures[type] = other->m_Textures.at(type);
	}
}

Material::~Material()
{
	delete m_pMaterialAttributes;
}

void Material::SetCBV(ConstantBufferView* cbv)
{
	m_pCbv = cbv;
}

void Material::SetTexture(TEXTURE_TYPE textureType, Texture* texture)
{
	m_Textures[textureType] = texture;
	
	switch (textureType)
	{
	case TEXTURE_TYPE::AMBIENT:
		m_pSlotInfo->textureAmbient = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::DIFFUSE:
		m_pSlotInfo->textureDiffuse = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::SPECULAR:
		m_pSlotInfo->textureSpecular = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::NORMAL:
		m_pSlotInfo->textureNormal = texture->GetDescriptorHeapIndex();
		break;
	case TEXTURE_TYPE::EMISSIVE:
		m_pSlotInfo->textureEmissive = texture->GetDescriptorHeapIndex();
		break;
	}
}

void Material::SetShininess(float shininess)
{
	m_pMaterialAttributes->shininess = shininess;
}

void Material::SetColorAdd(COLOR_TYPE type, float4 color)
{
	switch (type)
	{
	case COLOR_TYPE::LIGHT_AMBIENT:
		m_pMaterialAttributes->ambientAdd = color;
		break;
	case COLOR_TYPE::LIGHT_DIFFUSE:
		m_pMaterialAttributes->diffuseAdd = color;
		break;
	case COLOR_TYPE::LIGHT_SPECULAR:
		m_pMaterialAttributes->specularAdd = color;
		break;
	}
}

void Material::SetColorMul(COLOR_TYPE type, float4 color)
{
	switch (type)
	{
	case COLOR_TYPE::LIGHT_AMBIENT:
		m_pMaterialAttributes->ambientMul = color;
		break;
	case COLOR_TYPE::LIGHT_DIFFUSE:
		m_pMaterialAttributes->diffuseMul = color;
		break;
	case COLOR_TYPE::LIGHT_SPECULAR:
		m_pMaterialAttributes->specularMul = color;
		break;
	}
}

void Material::SetUVScale(float u, float v)
{
	m_pMaterialAttributes->uvScale = {u, v};
}

Texture* Material::GetTexture(TEXTURE_TYPE textureType)
{
	return m_Textures.at(textureType);
}

MaterialAttributes* Material::GetMaterialAttributes() const
{
	return m_pMaterialAttributes;
}

const ConstantBufferView* const Material::GetConstantBufferView() const
{
	return m_pCbv;
}
