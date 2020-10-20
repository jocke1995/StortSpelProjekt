#include "Headers/stdafx.h"
#include "ECS/Entity.h"
#include "MeleeComponent.h"
#include "HealthComponent.h"
#include "../Renderer/BoundingBoxPool.h"
#include "../ECS/Components/Audio2DVoiceComponent.h"


component::MeleeComponent::MeleeComponent(Entity* parent) : Component(parent)
{
	m_Attacking = false;
	m_Cooldown = false;
	m_AttackInterval = 0.2;
	m_TimeSinceLastAttackCheck = 0;
	m_pMesh = nullptr;
	m_Damage = 1;

	//Create bounding box for collision for melee
	m_pBbc = parent->GetComponent<component::BoundingBoxComponent>();
	createCornersHitbox();
	m_TempHitbox.CreateFromPoints(m_TempHitbox, 8, m_Corners, sizeof(DirectX::XMFLOAT3));
	m_Hitbox = m_TempHitbox;

	// Fetch the player transform
	m_pMeleeTransform = parent->GetComponent<component::TransformComponent>()->GetTransform();

	// Fetch the player audio component
	m_pVoiceComponent = parent->GetComponent<component::Audio2DVoiceComponent>();
	m_pVoiceComponent->AddVoice(L"SwordSwing");

	//Debugging purpose
	if (DEVELOPERMODE_DRAWBOUNDINGBOX)
	{
		createDrawnHitbox(m_pBbc);
	}
}

component::MeleeComponent::~MeleeComponent()
{
}

void component::MeleeComponent::OnInitScene()
{
}

void component::MeleeComponent::OnLoadScene()
{
}

void component::MeleeComponent::OnUnloadScene()
{
}

void component::MeleeComponent::Update(double dt)
{
	// Takes the transform of the player cube and moves it forward to act as a hitbox
	m_MeleeTransformModified = *m_pMeleeTransform;
	double3 modelDim = m_pParent->GetComponent<component::ModelComponent>()->GetModelDim();
	modelDim *= 0.5;
	float positonX = m_MeleeTransformModified.GetPositionFloat3().x + (modelDim.x + 1.0) * m_MeleeTransformModified.GetRotMatrix().r[2].m128_f32[0];
	float positonY = m_MeleeTransformModified.GetPositionFloat3().y + (modelDim.y + 1.0) * m_MeleeTransformModified.GetRotMatrix().r[2].m128_f32[1];
	float positonZ = m_MeleeTransformModified.GetPositionFloat3().z + (modelDim.z + 1.0) * m_MeleeTransformModified.GetRotMatrix().r[2].m128_f32[2];
	
	// Sets the position and updates the matrix to reflect movement of the player
	m_MeleeTransformModified.SetPosition(positonX, positonY, positonZ);
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
			Log::Print("Attack off cooldown \n");
			m_Attacking = false;
			m_Cooldown = false;
		}
		m_TimeSinceLastAttackCheck = 0;
	}

	// Updates the hitzone to follow the player
	temp.Transform(temp, *m_MeleeTransformModified.GetWorldMatrix());
	m_Hitbox = temp;

}

void component::MeleeComponent::Attack(bool attack)
{
	
	if (!m_Cooldown)
	{
		m_pVoiceComponent->Play(L"SwordSwing");
		Log::Print("Attacking now \n");
		m_Attacking = attack;
		//Checks collision of entities
		CheckCollision();
		m_Cooldown = true;
		m_TimeSinceLastAttackCheck = 0;
	}
}

void component::MeleeComponent::setAttackInterval(float interval)
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

void component::MeleeComponent::CheckCollision()
{
	std::vector<Entity*> list = Physics::GetInstance().SpecificCollisionCheck(&m_Hitbox);
	for (unsigned int i = 0; i < list.size(); i++) 
	{
		// Checks if the collision occurs on something with a healthcomponent and is not the player themselves
		if (list.at(i)->GetName() != "player" && list.at(i)->GetComponent<component::HealthComponent>() != nullptr)
		{
			list.at(i)->GetComponent<component::HealthComponent>()->ChangeHealth(-m_Damage);
		}
	}
	list.empty();
}

void component::MeleeComponent::createCornersHitbox()
{
	//Create position for each corner of the hitbox
	// Front vertices
	m_Corners[0].x =  3;	m_Corners[0].y =  1;	m_Corners[0].z = -1;
	m_Corners[1].x =  3;	m_Corners[1].y = -1;	m_Corners[1].z = -1;
	m_Corners[2].x = -3;	m_Corners[2].y = -1;	m_Corners[2].z = -1;
	m_Corners[3].x = -3;	m_Corners[3].y =  1;	m_Corners[3].z = -1;
	// Back vertices
	m_Corners[4].x =  3;	m_Corners[4].y =  1;	m_Corners[4].z = 2;
	m_Corners[5].x =  3;	m_Corners[5].y = -1;	m_Corners[5].z = 2;
	m_Corners[6].x = -3;	m_Corners[6].y = -1;	m_Corners[6].z = 2;
	m_Corners[7].x = -3;	m_Corners[7].y =  1;	m_Corners[7].z = 2;
}

void component::MeleeComponent::createDrawnHitbox(component::BoundingBoxComponent* bbc)
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

