#include "RangeComponent.h"
#include "ECS/Scene.h"
#include "Engine.h"
#include "Events/Events.h"
#include "../ECS/SceneManager.h"

component::RangeComponent::RangeComponent(Entity* parent, SceneManager* sm, Scene* scene, Model* model, int damage, float velocity) : Component(parent)
{
	m_pSceneMan = sm;
	m_pScene = scene;
	m_pModel = model;
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

		mc = ent->AddComponent<component::ModelComponent>();
		tc = ent->AddComponent<component::TransformComponent>();

		float3 pos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
		//DirectX::XMFLOAT3 forward = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetForwardVec();
		DirectX::XMFLOAT3 forward = m_pScene->GetMainCamera()->GetDirection();
		mc->SetModel(m_pModel);
		mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
		tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);//pos.x + (forward.x * 2), pos.y + (forward.y * 2), pos.z + (forward.z * 2));
		tc->GetTransform()->SetScale(m_Scale);
		tc->GetTransform()->SetVelocity(m_Velocity);
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		Physics::GetInstance().AddCollisionEntity(ent);

		m_pSceneMan->AddEntity(ent);
	}

}
