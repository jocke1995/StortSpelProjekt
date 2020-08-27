#ifndef ENTITY_H
#define ENTITY_H

// Renderer
#include "Components/MeshComponent.h"
#include "Components/TransformComponent.h"
#include "Components/CameraComponent.h"
#include "Components/BoundingBoxComponent.h"

// Lights
class DirectionalLightComponent;
class PointLightComponent;
class SpotLightComponent;

static unsigned int staticID = 0;
class Entity
{
public:
	Entity(std::string entityName);
	bool operator == (const Entity* rhs) const;
	~Entity();

	template <class T, typename... Args>
	T* AddComponent(Args... args);

	template <class T>
	T* GetComponent() const;

	template <class T>
	bool HasComponent() const;

	unsigned int GetID() const;
	std::string GetName() const;
	unsigned int GetRefCount() const;

	void IncrementRefCount();
	void DecrementRefCount();
	void Update(double dt);

private:
	unsigned int id = -1;
	std::string name = "";
	
	// Multiple scenes can use the same entity (player for example).
	// This is to make sure that the player doesn't get deleted if its still in use AND to not delete it twice
	unsigned int referenceCount = 0;

	std::vector<Component*> components;
};

template<class T, typename... Args>
inline T* Entity::AddComponent(Args... args)
{
	// Check if component already exists,
	// and if it does.. return it
	T* compInEntity = this->GetComponent<T>();
	if (compInEntity == nullptr)
	{
		// Add component
		T* finalComponent = new T(this, std::forward<Args>(args)...);
		this->components.push_back(finalComponent);

		return finalComponent;
	}
	return compInEntity;
}

template<class T>
inline T* Entity::GetComponent() const
{
	for (int i = 0; i < this->components.size(); i++)
	{
		T* component = dynamic_cast<T*>(this->components[i]);

		if (component != nullptr)
		{
			return component;
		}
	}
	return nullptr;
}

template<class T>
inline bool Entity::HasComponent() const
{
	for (int i = 0; i < this->components.size(); i++)
	{
		T* component = dynamic_cast<T*>(this->components[i]);

		if (component != nullptr)
		{
			// Found
			return true;
		}
	}
	return false;
}

#endif
