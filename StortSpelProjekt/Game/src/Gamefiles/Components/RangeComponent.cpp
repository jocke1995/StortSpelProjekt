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
		DirectX::XMFLOAT3 forward = m_pScene->GetMainCamera()->GetDirection(); //m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetForwardVec();//m_pScene->GetMainCamera()->GetDirection();

		float rotPosX = (pos.x + (forward.x * 1));
		float rotPosY = (pos.y + (forward.y * 1));
		float rotPosZ = (pos.z + (forward.z * 1));

		float3 camPos = m_pScene->GetMainCamera()->GetPositionFloat3();
		DirectX::XMFLOAT3 origDir = {0.0f, 0.0f, 1.0f};//m_pScene->GetMainCamera()->GetDirection();
		DirectX::XMVECTOR oD = DirectX::XMLoadFloat3(&origDir);
		DirectX::XMVECTOR fD = DirectX::XMLoadFloat3(&forward);
		DirectX::XMVECTOR angle = DirectX::XMVector3AngleBetweenVectors(oD, fD);
		DirectX::XMVECTOR axle = DirectX::XMVector3Cross(oD, fD);
		DirectX::XMFLOAT3 fAxle;
		DirectX::XMStoreFloat3(&fAxle, axle);
		float angleX = std::atan2(forward.y, forward.z);
		//float angleY = std::atan2(forward.x, forward.z);
		DirectX::XMFLOAT3 fAngle;
		DirectX::XMStoreFloat3(&fAngle, angle);
		Log::Print("x: %f\n", fAngle.x);
		Log::Print("y: %f\n", fAngle.y);
		Log::Print("z: %f\n", fAngle.z);
		//fAngle.x *= forward.x / abs(forward.x);
		//fAngle.y *= forward.x / abs(forward.x);
		//fAngle.z *= forward.x / abs(forward.x);
		
		//DirectX::XMFLOAT3 dir = { 0.0f, 0.0f, 1.0f };		
		//DirectX::XMVECTOR fdd = DirectX::XMLoadFloat3(&dir);
		//DirectX::XMVECTOR vec = DirectX::XMVector3Transform(fdd, *m_pScene->GetMainCamera()->GetViewMatrixInverse());
		//DirectX::XMFLOAT3 vecF;
		//DirectX::XMStoreFloat3(&vecF, vec);

		//DirectX::XMFLOAT3 cPos;
		//cPos.x = camPos.x;
		//cPos.y = camPos.y;
		//cPos.z = camPos.z;
		//DirectX::XMVECTOR orig = 
		float3 orig;
		orig.x = pos.x + forward.x;
		orig.y = pos.y + forward.y;
		orig.z = pos.z + forward.z;

		//float3 vel = DirectX::XMVector3Dot(forward, m_Velocity);

		mc->SetModel(m_pModel);
		mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
		tc->GetTransform()->SetPosition(orig.x, orig.y, orig.z);
		tc->Update(0.02);
		tc->GetTransform()->SetRotationX(angleX);
		//tc->GetTransform()->SetRotationY(angleY);
		//tc->GetTransform()->SetRotationZ(fAngle.z);
		tc->GetTransform()->SetMovement(forward.x, forward.y, forward.z);
		tc->GetTransform()->SetScale(m_Scale);
		tc->GetTransform()->SetVelocity(m_Velocity); 
		tc->Update(0.02);
		tc->GetTransform()->SetPosition(rotPosX, rotPosY, rotPosZ);
		tc->Update(0.02);
		bbc = ent->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
		bbc->Init();
		Physics::GetInstance().AddCollisionEntity(ent);

		m_pSceneMan->AddEntity(ent);
	}

}
