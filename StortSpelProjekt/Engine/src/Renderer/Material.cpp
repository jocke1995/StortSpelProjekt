#include "stdafx.h"
#include "Material.h"

Material::Material(SlotInfo* slotInfo)
{
	this->slotInfo = slotInfo;
}

Material::~Material()
{
	
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

Texture* Material::GetTexture(TEXTURE_TYPE textureType)
{
	return this->textures.at(textureType);
}
