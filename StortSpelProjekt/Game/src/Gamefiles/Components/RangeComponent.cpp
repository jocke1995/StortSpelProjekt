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

void component::RangeComponent::Attack(MouseClick* event)
{
	if (event->button == MOUSE_BUTTON::RIGHT_DOWN)
	{
		m_NrOfProjectiles++;
		Entity* ent = m_pScene->AddEntity("RangeAttack" + std::to_string(m_NrOfProjectiles));
		component::ModelComponent* mc = nullptr;
		component::TransformComponent* tc = nullptr;
		component::BoundingBoxComponent* bbc = nullptr;
		component::AccelerationComponent* ac = nullptr;
		component::ProjectileComponent* pc = nullptr;
		component::UpgradeComponent* uc = nullptr;

		mc = ent->AddComponent<component::ModelComponent>();
		tc = ent->AddComponent<component::TransformComponent>();
		pc = ent->AddComponent<component::ProjectileComponent>(m_Damage);
		ac = ent->AddComponent <component::AccelerationComponent>(50);
		uc = ent->AddComponent<component::UpgradeComponent>();

		// Applying all range uppgrades to the new projectile entity "RangeAttack"
		Player::GetInstance().GetUpgradeManager()->ApplyRangeUpgrades(ent);
		

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
	}

}
