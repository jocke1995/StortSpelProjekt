#include "RangeComponent.h"
#include "ECS/Scene.h"
#include "Engine.h"
#include "Events/Events.h"
#include "../ECS/SceneManager.h"
#include "ProjectileComponent.h"


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
		component::ProjectileComponent* pc = nullptr;

		mc = ent->AddComponent<component::ModelComponent>();
		tc = ent->AddComponent<component::TransformComponent>();
		pc = ent->AddComponent<component::ProjectileComponent>(m_Damage);

		float3 pos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
		DirectX::XMFLOAT3 forward = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetForwardVec();//m_pScene->GetMainCamera()->GetDirection();

		float rotPosX = (pos.x + (forward.x * 0.5));
		float rotPosY = (pos.y + (forward.y * 0.5));
		float rotPosZ = (pos.z + (forward.z * 0.5));

		DirectX::XMFLOAT3 camDir = m_pScene->GetMainCamera()->GetDirection();
		DirectX::XMVECTOR cD = DirectX::XMLoadFloat3(&camDir);
		DirectX::XMVECTOR fD = DirectX::XMLoadFloat3(&forward);
		DirectX::XMVECTOR angle = DirectX::XMVector3AngleBetweenVectors(cD, fD);
		DirectX::XMFLOAT3 fAngle;
		DirectX::XMStoreFloat3(&fAngle, angle);

		mc->SetModel(m_pModel);
		mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
		tc->GetTransform()->SetPosition(rotPosX, rotPosY, rotPosZ);
		tc->GetTransform()->SetRotationX((forward.x) + fAngle.x);
		tc->GetTransform()->SetRotationY((forward.y - 3.4) + fAngle.y);
		tc->GetTransform()->SetRotationZ((forward.z) + fAngle.z);
		tc->GetTransform()->SetMovement(forward.x, forward.y, forward.z);
		tc->GetTransform()->SetScale(m_Scale);
		tc->GetTransform()->SetVelocity(m_Velocity); 
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		tc->GetTransform()->UpdateWorldMatrix();
		Physics::GetInstance().AddCollisionEntity(ent);
		tc->Update(0.02);
		m_pSceneMan->AddEntity(ent);
	}

}
