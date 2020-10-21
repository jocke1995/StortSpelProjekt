#include "stdafx.h"
#include "Material.h"

Material::Material(const std::wstring* name, std::map<TEXTURE2D_TYPE, Texture*>* textures)
{
	m_Name = *name;

	// copy the texture pointers
	m_Textures = *textures;
}

Material::~Material()
{

}

const std::wstring& Material::GetPath() const
{
	return m_Name;
}

Texture* Material::GetTexture(TEXTURE2D_TYPE type) const
{
	return m_Textures.at(type);
}

void Material::SetTexture(TEXTURE2D_TYPE type, Texture* texture)
{
	m_Textures[type] = texture;
}
