#include "stdafx.h"
#include "Material.h"

Material::Material(const std::wstring* path, std::map<TEXTURE_TYPE, Texture*>* textures)
{
	m_Name = *path;

	// copy the texture pointers
	m_Textures = *textures;
}

Material::~Material()
{
}

const std::wstring* Material::GetPath() const
{
	return &m_Name;
}

Texture* Material::GetTexture(unsigned int index) const
{
	return m_Textures.at(static_cast<TEXTURE_TYPE>(index));
}