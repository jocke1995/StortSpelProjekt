#include "Headers/stdafx.h"
#include "ECS/Entity.h"
#include "MeleeComponent.h"
#include "HealthComponent.h"
#include "../Renderer/BoundingBoxPool.h"


component::MeleeComponent::MeleeComponent(Entity* parent) : Component(parent)
{
	m_Attacking = false;
	m_Cooldown = false;
	m_AttackIntervall = 1.0;
	m_TimeSinceLastAttackCheck = 0;
	m_pMesh = nullptr;

	//Create bounding box for collision for melee
	m_pBbc = parent->GetComponent<component::BoundingBoxComponent>();
	createCornersHitbox();
	m_TempHitbox.CreateFromPoints(m_TempHitbox, 8, m_Corners, sizeof(DirectX::XMFLOAT3));
	m_Hitbox = m_TempHitbox;

	// Fetch the player transform
	m_pMeleeTransform = parent->GetComponent<component::TransformComponent>()->GetTransform();

	//Debugging purpose
	if (DEVELOPERMODE_DRAWBOUNDINGBOX)
	{
		createDrawnHitbox(m_pBbc);
	}
}

component::MeleeComponent::~MeleeComponent()
{
}

void component::MeleeComponent::Update(double dt)
{
	m_MeleeTransformTwo = *m_pMeleeTransform;
	float positonX = m_MeleeTransformTwo.GetPositionFloat3().x + 2*m_MeleeTransformTwo.GetRotMatrix().r[2].m128_f32[0];
	float positonY = m_MeleeTransformTwo.GetPositionFloat3().y + 2*m_MeleeTransformTwo.GetRotMatrix().r[2].m128_f32[1];
	float positonZ = m_MeleeTransformTwo.GetPositionFloat3().z + 2*m_MeleeTransformTwo.GetRotMatrix().r[2].m128_f32[2];
	
	m_MeleeTransformTwo.SetPosition(positonX, positonY, positonZ);
	m_MeleeTransformTwo.Move(dt);
	m_MeleeTransformTwo.UpdateWorldMatrix();

	DirectX::BoundingOrientedBox temp;
	temp = m_TempHitbox;

	m_TimeSinceLastAttackCheck += dt;
	if (m_TimeSinceLastAttackCheck > m_AttackIntervall)
	{
		if (m_Attacking == true)
		{
			Log::Print("Attack off cooldown \n");
			m_Attacking = false;
			m_Cooldown = false;
		}
		m_TimeSinceLastAttackCheck = 0;
	}
	temp.Transform(temp, *m_MeleeTransformTwo.GetWorldMatrix());
	m_Hitbox = temp;

}

void component::MeleeComponent::Attack(bool attack)
{
	if (!m_Cooldown)
	{
		Log::Print("Attacking now \n");
		m_Attacking = attack;
		CheckCollision();
		m_Cooldown = true;
	}
}

void component::MeleeComponent::setAttackIntervall(float intervall)
{
	m_AttackIntervall = intervall;
}

void component::MeleeComponent::CheckCollision()
{
	std::vector<Entity*> list = Physics::GetInstance().SpecificCollisionCheck(&m_Hitbox);
	if (list.size() != 0)
	{
		for (int i = 0; i < list.size(); i++) 
		{
			if (list.at(i)->GetName() != "player" && list.at(i)->GetComponent<component::HealthComponent>() != nullptr)
			{
				list.at(i)->GetComponent<component::HealthComponent>()->ChangeHealth(-100);
			}
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


	for (unsigned int i = 0; i < 8; i++)
	{
		m_BoundingBoxVerticesLocal.push_back(v[i]);
	}

	// Indices
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

	for (unsigned int i = 0; i < 36; i++)
	{
		m_BoundingBoxIndicesLocal.push_back(indices[i]);
	}

	BoundingBoxData bbd = {};
	bbd.boundingBoxVertices = m_BoundingBoxVerticesLocal;
	bbd.boundingBoxIndices = m_BoundingBoxIndicesLocal;

	bbc->AddBoundingBox(&bbd, &m_MeleeTransformTwo, L"sword");
}

