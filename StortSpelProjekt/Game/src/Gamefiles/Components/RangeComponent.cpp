#include "RangeComponent.h"
#include "ECS/Scene.h"
#include "Events/Events.h"
#include "../ECS/SceneManager.h"
#include "ProjectileComponent.h"
#include <Bullet/btBulletCollisionCommon.h>



component::RangeComponent::RangeComponent(Entity* parent, SceneManager* sm, Scene* scene, Model* model, float scale, int damage, float velocity) : Component(parent)
{
	m_pSceneMan = sm;
	m_pScene = scene;
	m_pModel = model;
	m_Scale = scale;
	m_Damage = damage;
	m_Velocity = velocity;
}

component::RangeComponent::~RangeComponent()
{
	m_ProjectileList.clear();
}

void component::RangeComponent::Attack()
{
	m_NrOfProjectiles++;
	Entity* ent = m_pScene->AddEntity("RangeAttack" + std::to_string(m_NrOfProjectiles));
	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;
	component::BoundingBoxComponent* bbc = nullptr;
	component::AccelerationComponent* ac = nullptr;
	component::ProjectileComponent* pc = nullptr;

	mc = ent->AddComponent<component::ModelComponent>();
	tc = ent->AddComponent<component::TransformComponent>();
	pc = ent->AddComponent<component::ProjectileComponent>(m_Damage);

	// get the pos of parent object and forward of camera 
	// so we know where to spawn and in which direction
	float3 ParentPos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
	DirectX::XMFLOAT3 forward = m_pScene->GetMainCamera()->GetDirection(); 

	// add the forward vector to parent pos 
	// so the projectile doesn't spawn inside of us
	float3 pos;
	pos.x = ParentPos.x + forward.x;
	pos.y = ParentPos.y + forward.y;
	pos.z = ParentPos.z + forward.z;

	// initialize the components
	mc->SetModel(m_pModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
	tc->GetTransform()->SetMovement(forward.x * m_Velocity, forward.y * m_Velocity, forward.z * m_Velocity);
	tc->GetTransform()->SetScale(m_Scale);
	tc->GetTransform()->SetVelocity(m_Velocity); 
	tc->Update(0.02);
	bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
	bbc->Init();
	Physics::GetInstance().AddCollisionEntity(ent);

	// add the entity to the sceneManager so it can be spawned in in run time
	m_pSceneMan->AddEntity(ent);

	// add entities to list to send to other clients if multiplayer
	m_ProjectileList.push_back(ent);

}

void component::RangeComponent::CreateNetworkProjectiles(float3 pos, float3 mov)
{
	m_NrOfProjectiles++;
	Entity* ent = m_pScene->AddEntity("RangeAttack" + std::to_string(m_NrOfProjectiles));
	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;

	mc = ent->AddComponent<component::ModelComponent>();
	tc = ent->AddComponent<component::TransformComponent>();

	// initialize the components
	mc->SetModel(m_pModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);
	tc->GetTransform()->SetMovement(mov.x,mov.y,mov.z);
	tc->GetTransform()->SetScale(1);
	tc->GetTransform()->SetVelocity(2.5f);
	tc->Update(0.02);

	// add the entity to the sceneManager so it can be spawned in in run time
	m_pSceneMan->AddEntity(ent);
}

std::vector<Entity*> component::RangeComponent::GetProjectileList()
{
	return m_ProjectileList;
}

void component::RangeComponent::ClearProjectileList()
{
	m_ProjectileList.clear();
}

