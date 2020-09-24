#include "stdafx.h"
#include "Material.h"

Material::Material(const std::string* path, std::map<TEXTURE_TYPE, Texture*>* textures)
{
	m_Path = *path;

	// copy the texture pointers
	m_Textures = *textures;
}

Material::~Material()
{
}

std::string Material::GetPath() const
{
	return m_Path;
}

Texture* Material::GetTexture(unsigned int index) const
{
	return m_Textures.at(static_cast<TEXTURE_TYPE>(index));
}