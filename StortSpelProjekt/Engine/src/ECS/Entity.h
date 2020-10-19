#ifndef ENTITY_H
#define ENTITY_H

// Renderer
#include "Components/ModelComponent.h"
#include "Components/TransformComponent.h"
#include "Components/CameraComponent.h"
#include "Components/BoundingBoxComponent.h"
#include "Components/GUI2DComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkyboxComponent.h"

// Audio
#include "Components/Audio2DVoiceComponent.h"
#include "Components/Audio3DListenerComponent.h"
#include "Components/Audio3DEmitterComponent.h"

// physics/collisions
#include "Components/Collision/SphereCollisionComponent.h"
#include "Components/Collision/CubeCollisionComponent.h"
#include "Components/Collision/CapsuleCollisionComponent.h"
#include "Components/Collision/HeightmapCollisionComponent.h"

#include "../../Game/src/Gamefiles/Components/GameComponents.h"

// Lights
#include "Components/Lights/PointLightComponent.h"
#include "Components/Lights/DirectionalLightComponent.h"
#include "Components/Lights/SpotLightComponent.h"


static unsigned int staticID = 0;
class Entity
{
public:
	Entity(std::string entityName);
	bool operator == (const Entity* rhs) const;
	virtual ~Entity();

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
	void RenderUpdate(double dt);

	void OnInitScene();

	std::vector<Component*>* GetAllComponents();

private:
	friend class SceneManager;

	unsigned int m_Id = -1;
	std::string m_Name = "";
	
	// Multiple m_pScenes can use the same entity (player for example).
	// This is to make sure that the player doesn't get deleted if its still in use AND to not delete it twice
	unsigned int m_ReferenceCount = 0;

	std::vector<Component*> m_Components;
};

template<class T, typename... Args>
inline T* Entity::AddComponent(Args... args)
{
	// Check if component already exists,
	// and if it does.. return it
	T* compInEntity = GetComponent<T>();
	if (compInEntity == nullptr)
	{
		// Add component
		T* finalComponent = new T(this, std::forward<Args>(args)...);
		m_Components.push_back(finalComponent);

		return finalComponent;
	}
	return compInEntity;
}

template<class T>
inline T* Entity::GetComponent() const
{
	for (int i = 0; i < m_Components.size(); i++)
	{
		T* component = dynamic_cast<T*>(m_Components[i]);

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
	for (int i = 0; i < m_Components.size(); i++)
	{
		T* component = dynamic_cast<T*>(m_Components[i]);

		if (component != nullptr)
		{
			// Found
			return true;
		}
	}
	return false;
}

#endif
