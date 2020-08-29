#include "stdafx.h"
#include "Material.h"

Material::Material(SlotInfo* slotInfo)
{
	this->slotInfo = slotInfo;
	this->materialAttributes = new MaterialAttributes();
}

Material::Material(const Material* other, SlotInfo* slotInfo)
{
	this->slotInfo = slotInfo;

	this->materialAttributes = new MaterialAttributes();
	this->materialAttributes->shininess = other->materialAttributes->shininess;
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

Texture* Material::GetTexture(TEXTURE_TYPE textureType)
{
	return this->textures.at(textureType);
}

const MaterialAttributes* Material::GetMaterialAttributes() const
{
	return this->materialAttributes;
}
