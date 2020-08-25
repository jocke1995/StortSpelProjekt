#ifndef BOUNDINGBOXCOMPONENT_H
#define BOUNDINGBOXCOMPONENT_H

#include "Component.h"
#include "../Entity.h"

// Creating the BB out of the model
#include "MeshComponent.h"

// Using the same transform as the models transform
#include "TransformComponent.h"

#include "Engine/Renderer/BoundingBoxPool.h"

namespace component
{
	class BoundingBoxComponent : public Component
	{
	public:
		BoundingBoxComponent(Entity* parent, bool pick = false, bool outlineWhenPicked = false);
		virtual ~BoundingBoxComponent();

		void Init();
		void Update(double dt);

		void SetMesh(Mesh* mesh);

		Transform* GetTransform() const;
		const Mesh* GetMesh() const;
		const BoundingBoxData* GetBoundingBoxData() const;
		const std::string GetPathOfModel() const;
		std::string GetParentName() const;

		bool Pick() const;
		bool Outline() const;

	private:
		std::string pathOfModel = "";
		BoundingBoxData* bbd = nullptr;
		Mesh* mesh = nullptr;

		bool pick = false;
		bool outlineWhenPicked = false;

		Transform* transform = nullptr;

		bool CreateBoundingBox();
	};
}

#endif
