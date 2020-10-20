#include "RangeComponent.h"
#include "ECS/Scene.h"
#include "Engine.h"
#include "Events/Events.h"
#include "../ECS/SceneManager.h"
#include "ProjectileComponent.h"
#include <Bullet/btBulletCollisionCommon.h>
#include "UpgradeComponents/UpgradeComponent.h"
#include "UpgradeComponents/Upgrades/UpgradeRangeTest.h"
#include "Player.h"
#include "../Memory/PoolAllocator.h"
#include "UpgradeManager.h"


component::RangeComponent::RangeComponent(Entity* parent, SceneManager* sm, Scene* scene, Model* model, float scale, int damage, float velocity) : Component(parent)
{
	m_pSceneMan = sm;
	m_pScene = scene;
	m_pModel = model;
	m_Scale = scale;
	m_Damage = damage;
	m_Velocity = velocity;
	EventBus::GetInstance().Subscribe(this, &RangeComponent::Attack);
}

component::RangeComponent::~RangeComponent()
{

}

void component::RangeComponent::OnInitScene()
{
}

void component::RangeComponent::OnUnInitScene()
{
}

void component::RangeComponent::SetVelocity(float velocity)
{
	m_Velocity = velocity;
}

float component::RangeComponent::GetVelocity()
{
	return m_Velocity;
}

void component::RangeComponent::Attack(MouseClick* event)
{
	if (event->button == MOUSE_BUTTON::RIGHT_DOWN)
	{
		Entity* ent = m_pScene->AddEntity("RangeAttack" + std::to_string(++m_NrOfProjectiles));
		component::ModelComponent* mc = nullptr;
		component::TransformComponent* tc = nullptr;
		component::BoundingBoxComponent* bbc = nullptr;
		component::ProjectileComponent* pc = nullptr;
		component::UpgradeComponent* uc = nullptr;
		component::AccelerationComponent* ac = nullptr;

		mc = ent->AddComponent<component::ModelComponent>();
		tc = ent->AddComponent<component::TransformComponent>();
		pc = ent->AddComponent<component::ProjectileComponent>(m_Damage);
		ac = ent->AddComponent<component::AccelerationComponent>(98.2);
		uc = ent->AddComponent<component::UpgradeComponent>();

		// Applying all range uppgrades to the new projectile entity "RangeAttack"
		if (m_pParent->HasComponent<component::UpgradeComponent>())
		{
			Player::GetInstance().GetUpgradeManager()->ApplyRangeUpgrades(ent);
		}

		// get the pos of parent object and forward of camera 
		// so we know where to spawn and in which direction
		float3 ParentPos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
		float3 forward = m_pScene->GetMainCamera()->GetDirectionFloat3();
		float length = forward.length();

		double3 dim = m_pParent->GetComponent<component::ModelComponent>()->GetModelDim();

		// add the forward vector to parent pos 
		// so the projectile doesn't spawn inside of us
		float3 pos;
		pos.x = ParentPos.x + (forward.x / length) * (dim.x / 2.0);
		pos.y = ParentPos.y + (forward.y / length);
		pos.z = ParentPos.z + (forward.z / length) * (dim.z / 2.0);

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
		// TODO: add dynamicly correct
		m_pSceneMan->AddEntity(ent, m_pScene);
	}
}

		