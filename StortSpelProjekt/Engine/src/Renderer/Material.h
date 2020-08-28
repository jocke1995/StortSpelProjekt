#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"

// For slotInfo
#include "structs.h"

class Material
{
public:
	Material(SlotInfo* slotInfo);
	virtual ~Material();

	// Sets
	void SetTexture(TEXTURE_TYPE textureType, Texture* texture);

	// Gets
	Texture* GetTexture(TEXTURE_TYPE textureType);

private:
	std::map<TEXTURE_TYPE, Texture*> textures;

	SlotInfo* slotInfo = nullptr;
};

#endif