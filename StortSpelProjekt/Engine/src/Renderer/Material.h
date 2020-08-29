#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"
#include "ConstantBufferView.h"

// For slotInfo
#include "structs.h"

class Material
{
public:
	Material(SlotInfo* slotInfo);
	Material(const Material* other, SlotInfo* slotInfo);
	virtual ~Material();

	// For usage in renderer
	void SetCBV(ConstantBufferView* cbv);

	// Sets
	void SetTexture(TEXTURE_TYPE textureType, Texture* texture);
	void SetShininess(float shininess);

	// Gets
	Texture* GetTexture(TEXTURE_TYPE textureType);
	const MaterialAttributes* GetMaterialAttributes() const;

private:
	std::map<TEXTURE_TYPE, Texture*> textures;

	SlotInfo* slotInfo = nullptr;

	ConstantBufferView* cbv = nullptr;
	MaterialAttributes* materialAttributes = nullptr;

};

#endif