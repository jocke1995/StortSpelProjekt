#include "Headers/stdafx.h"
#include "ECS/Entity.h"
#include "MeleeComponent.h"
#include "HealthComponent.h"
#include "../Renderer/BoundingBoxPool.h"
#include "../Renderer/Camera/PerspectiveCamera.h"

#include "../ECS/Components/Audio3DEmitterComponent.h"
#include "../ECS/Components/Audio2DVoiceComponent.h"


component::MeleeComponent::MeleeComponent(Entity* parent) : Component(parent)
{
	m_Attacking = false;
	m_Cooldown = false;
	m_AttackInterval = 0.2;
	m_TimeSinceLastAttackCheck = 0;
	m_pMesh = nullptr;
	m_Damage = 1;
	m_KnockBack = 5;

	m_HalfSize = { 8.0f, 1.0f, MELEE_RANGE / 2.0 };

	//Create bounding box for collision for melee
	m_pBbc = parent->GetComponent<component::BoundingBoxComponent>();
	CreateCornersHitbox();
	m_TempHitbox.CreateFromPoints(m_TempHitbox, 8, m_Corners, sizeof(DirectX::XMFLOAT3));
	m_Hitbox = m_TempHitbox;

	// Fetch the player transform
	m_pMeleeTransform = parent->GetComponent<component::TransformComponent>()->GetTransform();

	if (parent->GetComponent<component::Audio2DVoiceComponent>())
	{
		m_AudioPlay = true;
		// Fetch the player audio component (if one exists)
		m_pVoiceComponent = parent->GetComponent<component::Audio2DVoiceComponent>();
		m_pVoiceComponent->AddVoice(L"SwordSwing");
	}
	else
	{
		m_AudioPlay = false;
	}
	

	//Debugging purpose
	if (DEVELOPERMODE_DRAWBOUNDINGBOX)
	{
		CreateDrawnHitbox(m_pBbc);
	}
}

component::MeleeComponent::~MeleeComponent()
{
}

void component::MeleeComponent::OnInitScene()
{
}

void component::MeleeComponent::OnUnInitScene()
{
}

void component::MeleeComponent::Update(double dt)
{
	// Takes the transform of the player cube and moves it forward to act as a hitbox
	m_MeleeTransformModified = *m_pMeleeTransform;
	Transform* trans = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();
	
	DirectX::BoundingOrientedBox obb;
	obb.Center = m_pBbc->GetOriginalOBB()->Center;
	obb.Extents = m_pBbc->GetOriginalOBB()->Extents;
	obb.Orientation = m_pBbc->GetOriginalOBB()->Orientation;

	// then do all the transformations on this temoporary OBB so we don't change the original state
	obb.Transform(obb, *m_MeleeTransformModified.GetWorldMatrix());

	bool t_pose = m_pBbc->GetFlagOBB() & F_OBBFlags::T_POSE;

	float positionX = obb.Center.x + (obb.Extents.x + ((static_cast<float>(!t_pose) * m_HalfSize.x + static_cast<float>(t_pose) * m_HalfSize.z) * trans->GetScale().x)) * m_MeleeTransformModified.GetForwardFloat3().x;
	float positionY = obb.Center.y + (obb.Extents.y + (m_HalfSize.y * trans->GetScale().y)) * m_MeleeTransformModified.GetForwardFloat3().y;
	float positionZ = obb.Center.z + (obb.Extents.z + (m_HalfSize.z * trans->GetScale().z)) * m_MeleeTransformModified.GetForwardFloat3().z;

	// Sets the position and updates the matrix to reflect movement of the player
	m_MeleeTransformModified.SetPosition(positionX, positionY, positionZ);
	m_MeleeTransformModified.Move(dt);
	m_MeleeTransformModified.UpdateWorldMatrix();

	DirectX::BoundingOrientedBox temp;
	temp = m_TempHitbox;

	//Check how long time has passed sinces last attack
	m_TimeSinceLastAttackCheck += dt;
	if (m_TimeSinceLastAttackCheck > m_AttackInterval)
	{
		//Checks if the player has attacked at some point. 
		if (m_Attacking == true)
		{
			// Sets the attacking state to false (=able to attack again)
			//Log::Print("Attack off cooldown \n");
			m_Attacking = false;
			m_Cooldown = false;
		}
		m_TimeSinceLastAttackCheck = 0;
	}

	// Updates the hitzone to follow the player
	temp.Transform(temp, *m_MeleeTransformModified.GetWorldMatrix());
	m_Hitbox = temp;

}

void component::MeleeComponent::Attack()
{
	
	if (!m_Cooldown)
	{
		if (m_AudioPlay)
		{
			m_pVoiceComponent->Play(L"SwordSwing");
		}
		//Log::Print("Attacking now \n");
		m_Attacking = true;
		//Checks collision of entities
		checkCollision();
		m_Cooldown = true;
		m_TimeSinceLastAttackCheck = 0;

		float3 forward = m_pParent->GetComponent<component::CameraComponent>()->GetCamera()->GetDirectionFloat3();

		// Makes player turn in direction of camera to attack
		double angle = std::atan2(forward.x, forward.z);
		int angleDegrees = EngineMath::convertToWholeDegrees(angle);
		angleDegrees = (angleDegrees + 360) % 360;
		m_pParent->GetComponent<component::PlayerInputComponent>()->SetAngleToTurnTo(angleDegrees);
		m_pParent->GetComponent<component::PlayerInputComponent>()->SetAttacking();
	}
}

void component::MeleeComponent::SetAttackInterval(float interval)
{
	m_AttackInterval = interval;
}

void component::MeleeComponent::SetDamage(int damage)
{
	m_Damage = damage;
}

void component::MeleeComponent::ChangeDamage(int change)
{
	m_Damage += change;
}

float component::MeleeComponent::GetAttackInterval()
{
	return m_AttackInterval;
}

void component::MeleeComponent::checkCollision()
{
	std::vector<Entity*> list = Physics::GetInstance().SpecificCollisionCheck(&m_Hitbox);
	for (unsigned int i = 0; i < list.size(); i++) 
	{
		// Checks if the collision occurs on something with a healthcomponent and is not the player themselves
		if (list.at(i)->GetName() != "player" && list.at(i)->GetComponent<component::HealthComponent>() != nullptr)
		{
			list.at(i)->GetComponent<component::Audio3DEmitterComponent>()->UpdateEmitter(L"Bruh");
			list.at(i)->GetComponent<component::Audio3DEmitterComponent>()->Play(L"Bruh");
			list.at(i)->GetComponent<component::HealthComponent>()->ChangeHealth(-m_Damage);
			list.at(i)->GetComponent<component::AiComponent>()->KnockBack(m_MeleeTransformModified, m_KnockBack);
		}
	}
	list.empty();
}

void component::MeleeComponent::CreateCornersHitbox()
{
	//Create position for each corner of the hitbox
	// Front vertices
	m_Corners[0].x =  m_HalfSize.x;	m_Corners[0].y =  m_HalfSize.y;	m_Corners[0].z = -m_HalfSize.z;
	m_Corners[1].x =  m_HalfSize.x;	m_Corners[1].y = -m_HalfSize.y;	m_Corners[1].z = -m_HalfSize.z;
	m_Corners[2].x = -m_HalfSize.x;	m_Corners[2].y = -m_HalfSize.y;	m_Corners[2].z = -m_HalfSize.z;
	m_Corners[3].x = -m_HalfSize.x;	m_Corners[3].y =  m_HalfSize.y;	m_Corners[3].z = -m_HalfSize.z;
	// Back vertices
	m_Corners[4].x =  m_HalfSize.x;	m_Corners[4].y =  m_HalfSize.y;	m_Corners[4].z = m_HalfSize.z;
	m_Corners[5].x =  m_HalfSize.x;	m_Corners[5].y = -m_HalfSize.y;	m_Corners[5].z = m_HalfSize.z;
	m_Corners[6].x = -m_HalfSize.x;	m_Corners[6].y = -m_HalfSize.y;	m_Corners[6].z = m_HalfSize.z;
	m_Corners[7].x = -m_HalfSize.x;	m_Corners[7].y =  m_HalfSize.y;	m_Corners[7].z = m_HalfSize.z;
}

void component::MeleeComponent::CreateDrawnHitbox(component::BoundingBoxComponent* bbc)
{
	// Create the drawn bounding box
	Vertex v[8] = {};

	// The vertices are the Corners of the OBB so send them
	// Front vertices
	v[0].pos = m_Corners[0];
	v[1].pos = m_Corners[1];
	v[2].pos = m_Corners[2];
	v[3].pos = m_Corners[3];

	// Back vertices
	v[4].pos = m_Corners[4];
	v[5].pos = m_Corners[5];
	v[6].pos = m_Corners[6];
	v[7].pos = m_Corners[7];

	//PUshing back the vertices to the vector
	for (unsigned int i = 0; i < 8; i++)
	{
		m_BoundingBoxVerticesLocal.push_back(v[i]);
	}

	// Indices 
	// These for each of the faces for the hitzone, hardcoded as there is no model to load from.
	unsigned int indices[36] = {};
	// Front Face
	indices[0] = 0; indices[1] = 1; indices[2] = 3;
	indices[3] = 1; indices[4] = 2; indices[5] = 3;

	// Back Face
	indices[6] = 4; indices[7] = 5; indices[8] = 7;
	indices[9] = 5; indices[10] = 6; indices[11] = 7;

	// Top Face
	indices[12] = 0; indices[13] = 3; indices[14] = 4;
	indices[15] = 3; indices[16] = 4; indices[17] = 7;

	// Bottom Face
	indices[18] = 1; indices[19] = 2; indices[20] = 5;
	indices[21] = 2; indices[22] = 5; indices[23] = 6;

	// Right Face
	indices[24] = 0; indices[25] = 1; indices[26] = 4;
	indices[27] = 1; indices[28] = 4; indices[29] = 5;

	// Left Face
	indices[30] = 2; indices[31] = 3; indices[32] = 7;
	indices[33] = 2; indices[34] = 6; indices[35] = 7;

	//Pushing back the indices to the vector
	for (unsigned int i = 0; i < 36; i++)
	{
		m_BoundingBoxIndicesLocal.push_back(indices[i]);
	}

	BoundingBoxData bbd = {};
	bbd.boundingBoxVertices = m_BoundingBoxVerticesLocal;
	bbd.boundingBoxIndices = m_BoundingBoxIndicesLocal;

	bbc->AddBoundingBox(&bbd, &m_MeleeTransformModified, L"sword");
}

void component::MeleeComponent::SetKnockBack(float knockBack)
{
	m_KnockBack = knockBack;
}

void component::MeleeComponent::ChangeKnockBack(float change)
{
	m_KnockBack += change;
}

