#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"

// For slotInfo
#include "structs.h"

class Material
{
public:
	Material(SlotInfo* slotInfo);
	Material(const Material* other);
	virtual ~Material();

	// Sets
	void SetTexture(TEXTURE_TYPE textureType, Texture* texture);
	void SetShininess(float shininess);

	// Gets
	Texture* GetTexture(TEXTURE_TYPE textureType);
	const MaterialAttributes* GetMaterialAttributes() const;

private:
	std::map<TEXTURE_TYPE, Texture*> textures;

	SlotInfo* slotInfo = nullptr;
	MaterialAttributes* materialAttributes = nullptr;
};

#endif