#include "stdafx.h"
#include "Material.h"

Material::Material(const std::wstring* path, std::map<TEXTURE2D_TYPE, Texture*>* textures)
{
	m_Name = *path;

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

Texture* Material::GetTexture(unsigned int index) const
{
	return m_Textures.at(static_cast<TEXTURE2D_TYPE>(index));
}