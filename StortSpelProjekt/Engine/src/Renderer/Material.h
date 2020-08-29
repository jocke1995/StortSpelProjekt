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

	// Sets
	void SetTexture(TEXTURE_TYPE textureType, Texture* texture);
	void SetShininess(float shininess);
	// For usage in renderer
	void SetCBV(ConstantBufferView* cbv);

	// Gets
	Texture* GetTexture(TEXTURE_TYPE textureType);
	MaterialAttributes* GetMaterialAttributes() const;
	const ConstantBufferView* const GetConstantBufferView() const;

private:
	std::map<TEXTURE_TYPE, Texture*> textures;

	SlotInfo* slotInfo = nullptr;

	// a constantBuffer containing the materialAttributesData
	ConstantBufferView* cbv = nullptr;
	MaterialAttributes* materialAttributes = nullptr;

};

#endif