#ifndef BOUNDINGBOXCOMPONENT_H
#define BOUNDINGBOXCOMPONENT_H

#include "Component.h"
#include "EngineMath.h"

#include <DirectXCollision.h>

#include <vector>

// used for enabling Collision and/or Picking.
// Y_AXIS_UPP is only used if the "up" axis of the model is in the Y axis
// write as "F_OBBFlags::COLLISION | F_OBBFlags::PICKING", without the "", if you want to have more than one
enum F_OBBFlags
{
	COLLISION = BIT(1),
	PICKING = BIT(2),
	T_POSE = BIT(3),
};

struct BoundingBoxData;
class ShaderResourceView;
class Mesh;
class Transform;
struct SlotInfo;
class CollisionCategory;

namespace component
{
	class BoundingBoxComponent : public Component
	{
	public:
		BoundingBoxComponent(Entity* parent, unsigned int flagOBB = 0);
		virtual ~BoundingBoxComponent();

		void Init();
		void OnInitScene();
		void OnUnInitScene();

		//updates the position and rotation of m_OrientedBoundingBox
		void Update(double dt);

		void AddMesh(Mesh* mesh);

		// This function allows for using the boundingBoxComponent from sibling components
		// Example usage: MeleeComponent needs to create a boundingBox for the area of attack
		void AddBoundingBox(BoundingBoxData* bbd, Transform* transform, std::wstring path);

		// Will write warning to Log if Collision is not enabled for object
		const DirectX::BoundingOrientedBox* GetOBB() const;
		Transform* GetTransformAt(unsigned int index) const;
		const Mesh* GetMeshAt(unsigned int index) const;
		const BoundingBoxData* GetBoundingBoxDataAt(unsigned int index) const;
		const unsigned int GetNumBoundingBoxes() const;
		const std::wstring GetPathOfModel(unsigned int index) const;
		const SlotInfo* GetSlotInfo(unsigned int index) const;
		unsigned int GetFlagOBB() const;
		const DirectX::BoundingOrientedBox* GetOriginalOBB() const;

		void SetModifier(float3 modifier);

		// Renderer calls this function when an entity is picked
		bool& IsPickedThisFrame();

		template<typename T>
		CollisionCategory* AddCollisionCategory();

		void Collide(const BoundingBoxComponent& other);

	private:
		// used for collision checks
		DirectX::BoundingOrientedBox m_OrientedBoundingBox;
		// inital state of the OBB, used for math in update()
		DirectX::BoundingOrientedBox m_OriginalBoundingBox;
		std::vector<Transform*> m_Transforms;
		// If picking and or collision should be enabled
		unsigned int m_FlagOBB = 0;
		std::vector<std::wstring> m_Identifier;
		std::vector<Mesh*> m_Meshes;
		float3 m_Modifier = { 1.0f, 1.0f, 1.0f };
		std::vector<BoundingBoxData*> m_Bbds;
		std::vector<SlotInfo*> m_SlotInfos;
		CollisionCategory* m_pCategory = nullptr;

		bool createOrientedBoundingBox();
	};

	template<typename T>
	inline CollisionCategory* BoundingBoxComponent::AddCollisionCategory()
	{
		delete m_pCategory;

		m_pCategory = new T(m_pParent);
		return m_pCategory;
	}
}

#endif
