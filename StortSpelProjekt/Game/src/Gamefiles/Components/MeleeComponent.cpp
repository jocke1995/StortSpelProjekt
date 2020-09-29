#include "Headers/stdafx.h"
#include "ECS/Entity.h"
#include "MeleeComponent.h"
#include "../Renderer/BoundingBoxPool.h"


component::MeleeComponent::MeleeComponent(Entity* parent) : Component(parent)
{
	DirectX::XMFLOAT3 corners[8];
	component::BoundingBoxComponent* bbc = parent->GetComponent<component::BoundingBoxComponent>();
	bbc->GetOBB()->GetCorners(corners);
	tempHitbox.CreateFromPoints(tempHitbox, 8, corners, sizeof(DirectX::XMFLOAT3));
	Hitbox = tempHitbox;

	// Create the drawn bounding box
	Vertex v[8] = {};

	// The vertices are the corners of the OBB so send them
	// Front vertices
	v[0].pos = corners[0];
	v[1].pos = corners[1];
	v[2].pos = corners[2];
	v[3].pos = corners[3];

	// Back vertices
	v[4].pos = corners[4];
	v[5].pos = corners[5];
	v[6].pos = corners[6];
	v[7].pos = corners[7];


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
	indices[6] = 6; indices[7] = 5; indices[8] = 4;
	indices[9] = 7; indices[10] = 6; indices[11] = 4;

	// Top Face
	indices[12] = 5; indices[13] = 6; indices[14] = 1;
	indices[15] = 1; indices[16] = 6; indices[17] = 2;

	// Bottom Face
	indices[18] = 3; indices[19] = 4; indices[20] = 0;
	indices[21] = 3; indices[22] = 7; indices[23] = 4;

	// Right Face
	indices[24] = 4; indices[25] = 5; indices[26] = 0;
	indices[27] = 5; indices[28] = 1; indices[29] = 0;

	// Left Face
	indices[30] = 3; indices[31] = 2; indices[32] = 7;
	indices[33] = 2; indices[34] = 6; indices[35] = 7;

	for (unsigned int i = 0; i < 36; i++)
	{
		m_BoundingBoxIndicesLocal.push_back(indices[i]);
	}

	BoundingBoxData bbd = {};
	bbd.boundingBoxVertices = m_BoundingBoxVerticesLocal;
	bbd.boundingBoxIndices = m_BoundingBoxIndicesLocal;

	bbc->AddBoundingBox(&bbd, &m_MeleeTransformTwo, L"sword");

	m_pMeleeTransform = parent->GetComponent<component::TransformComponent>()->GetTransform();

	m_Attacking = false;
	m_Cooldown = false;
	m_attackIntervall = 3.0;
	m_timeSinceLastAttackCheck = 0;
	collide = nullptr;
	m_pMesh = nullptr;
}

component::MeleeComponent::~MeleeComponent()
{
}

void component::MeleeComponent::Update(double dt)
{
	m_MeleeTransformTwo = *m_pMeleeTransform;
	float positonX = m_MeleeTransformTwo.GetPositionFloat3().x;
	float positonY = m_MeleeTransformTwo.GetPositionFloat3().y;
	float positonZ = m_MeleeTransformTwo.GetPositionFloat3().z + 2;
	m_MeleeTransformTwo.SetPosition(positonX, positonY, positonZ);
	m_MeleeTransformTwo.Move(dt);
	m_MeleeTransformTwo.UpdateWorldMatrix();

	DirectX::BoundingOrientedBox temp;
	temp = tempHitbox;

	m_timeSinceLastAttackCheck += dt;
	if (m_timeSinceLastAttackCheck > m_attackIntervall)
	{
		if (m_Attacking == true)
		{
			Log::Print("Attack off cooldown \n");
			m_Attacking = false;
			m_Cooldown = false;
		}
		m_timeSinceLastAttackCheck = 0;
	}
	temp.Transform(temp, *m_MeleeTransformTwo.GetWorldMatrix());
	Hitbox = temp;

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

void component::MeleeComponent::AddEntityRock(Entity* ent)
{
	collide = ent;
}

void component::MeleeComponent::CheckCollision()
{
	if (collide->GetComponent<component::BoundingBoxComponent>()->GetOBB()->Intersects(Hitbox))
	{
		Log::Print("Melee Component collied \n");
	}
}

