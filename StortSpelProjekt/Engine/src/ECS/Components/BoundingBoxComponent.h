#ifndef BOUNDINGBOXCOMPONENT_H
#define BOUNDINGBOXCOMPONENT_H

#include "Component.h"
#include "EngineMath.h"
#include<DirectXCollision.h>

// used for enabling Collision and/or Picking.
// write as "F_OBB_Flags::Collision | F_OBB_Flags::Picking", without the "", if you want to have both
enum F_OBB_Flags
{
	Collision = BIT(1),
	Picking = BIT(2),

};

struct BoundingBoxData;
class Mesh;
class Transform;
namespace component
{
	class BoundingBoxComponent : public Component
	{
	public:
		// The F_OBB_Flags is for enabling collision and/or picking.
		// Write as "F_OBB_Flags::Collision | F_OBB_Flags::Picking", without the "", if you want to have both
		BoundingBoxComponent(Entity* parent, unsigned int flagOBB = 0);
		virtual ~BoundingBoxComponent();

		void Init();
		//updates the position and rotation of m_OrientedBoundingBox
		void Update(double dt);

		void SetMesh(Mesh* mesh);

		DirectX::BoundingOrientedBox GetOBB() const;
		Transform* GetTransform() const;
		const Mesh* GetMesh() const;
		const BoundingBoxData* GetBoundingBoxData() const;
		const std::string GetPathOfModel() const;
		unsigned int GetFlagOBB() const;

		// Renderer calls this function when an entity is picked
		bool& IsPickedThisFrame();

	private:
		DirectX::BoundingOrientedBox m_OrientedBoundingBox;
		Transform* m_pTransform = nullptr;
		// If picking and or collision should be enabled
		unsigned int m_FlagOBB = 0;
		Mesh* m_pMesh = nullptr;
		std::string m_pPathOfModel = "";
		BoundingBoxData* m_pBbd = nullptr;


		bool createOrientedBoundingBox();

	};
}

#endif
