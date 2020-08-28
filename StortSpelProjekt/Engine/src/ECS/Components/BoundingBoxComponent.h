#ifndef BOUNDINGBOXCOMPONENT_H
#define BOUNDINGBOXCOMPONENT_H

#include "Component.h"
#include "../Entity.h"

// Creating the BB out of the model
#include "MeshComponent.h"

// Using the same transform as the models transform
#include "TransformComponent.h"

#include "../Renderer/BoundingBoxPool.h"

namespace component
{
	class BoundingBoxComponent : public Component
	{
	public:
		BoundingBoxComponent(Entity* parent, bool pick = false);
		virtual ~BoundingBoxComponent();

		void Init();
		void Update(double dt);

		void SetMesh(Mesh* mesh);

		Transform* GetTransform() const;
		const Mesh* GetMesh() const;
		const BoundingBoxData* GetBoundingBoxData() const;
		const std::string GetPathOfModel() const;

		bool CanBePicked() const;

		// Renderer calls this function when an entity is picked
		bool& IsPickedThisFrame();

	private:
		std::string pathOfModel = "";
		BoundingBoxData* bbd = nullptr;
		Mesh* mesh = nullptr;

		bool canBePicked = false;

		Transform* transform = nullptr;

		bool CreateBoundingBox();
	};
}

#endif
