#ifndef MATERIAL_H
#define MATERIAL_H

#include "Core.h"
// For slotInfo and MaterialAttributes
#include "structs.h"

#include <map>

// Forward Declarations
class Texture;
class ConstantBufferView;

class Material
{
public:
	Material(SlotInfo* slotInfo);
	Material(const Material* other, SlotInfo* slotInfo);
	virtual ~Material();

	// Sets
	void SetTexture(TEXTURE_TYPE textureType, Texture* texture);
	void SetShininess(float shininess);
	void SetColorAdd(COLOR_TYPE type, float4 color);
	void SetColorMul(COLOR_TYPE type, float4 color);
	void SetUVScale(float u, float v);

	// For usage in m_pRenderer
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