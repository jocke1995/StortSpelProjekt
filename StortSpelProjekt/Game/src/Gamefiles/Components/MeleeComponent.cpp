#include "Headers/stdafx.h"
#include "ECS/Entity.h"
#include "MeleeComponent.h"
#include "../Renderer/BoundingBoxPool.h"


component::MeleeComponent::MeleeComponent(Entity* parent) : Component(parent)
{
	m_Attacking = false;
	m_Cooldown = false;
	m_attackIntervall = 1.0;
	m_timeSinceLastAttackCheck = 0;
	m_pMesh = nullptr;

	//Create bounding box for collision for melee
	bbc = parent->GetComponent<component::BoundingBoxComponent>();
	createCornersHitbox();
	tempHitbox.CreateFromPoints(tempHitbox, 8, corners, sizeof(DirectX::XMFLOAT3));
	Hitbox = tempHitbox;

	// Fetch the player transform
	m_pMeleeTransform = parent->GetComponent<component::TransformComponent>()->GetTransform();

	//Debugging purpose
	if (DEVELOPERMODE_DRAWBOUNDINGBOX)
	{
		createDrawnHitbox(bbc);
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

void component::MeleeComponent::CheckCollision()
{
	std::vector<Entity*> list = Physics::GetInstance().SpecificCollisionCheck(&Hitbox);
	if (list.size() != 0)
	{
		for(int i = 0; i < list.size(); i++)
		Log::Print("Melee Component collied with %s\n", list.at(i)->GetName().c_str());
	}
}

void component::MeleeComponent::createCornersHitbox()
{
	//Create position for each corner of the hitbox
	// Front vertices
	corners[0].x =  3;	corners[0].y =  1;	corners[0].z = 0;
	corners[1].x =  3;	corners[1].y = -1;	corners[1].z = 0;
	corners[2].x = -3;	corners[2].y =  1;	corners[2].z = 0;
	corners[3].x = -3;	corners[3].y = -1;	corners[3].z = 0;
	// Back vertices
	corners[4].x =  3;	corners[4].y = -1;	corners[4].z = 2;
	corners[5].x =  3;	corners[5].y =  1;	corners[5].z = 2;
	corners[6].x = -3;	corners[6].y = -1;	corners[6].z = 2;
	corners[7].x = -3;	corners[7].y =  1;	corners[7].z = 2;
}

void component::MeleeComponent::createDrawnHitbox(component::BoundingBoxComponent* bbc)
{
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
	indices[0] = 1; indices[1] = 3; indices[2] = 2;
	indices[3] = 0; indices[4] = 3; indices[5] = 2;

	// Back Face
	indices[6] = 4; indices[7] = 6; indices[8] = 5;
	indices[9] = 6; indices[10] = 7; indices[11] = 5;

	// Top Face
	indices[12] = 4; indices[13] = 6; indices[14] = 0;
	indices[15] = 6; indices[16] = 2; indices[17] = 0;

	// Bottom Face
	indices[18] = 5; indices[19] = 7; indices[20] = 1;
	indices[21] = 7; indices[22] = 3; indices[23] = 1;

	// Right Face
	indices[24] = 4; indices[25] = 0; indices[26] = 5;
	indices[27] = 0; indices[28] = 1; indices[29] = 5;

	// Left Face
	indices[30] = 6; indices[31] = 2; indices[32] = 7;
	indices[33] = 2; indices[34] = 3; indices[35] = 7;

	for (unsigned int i = 0; i < 36; i++)
	{
		m_BoundingBoxIndicesLocal.push_back(indices[i]);
	}

	BoundingBoxData bbd = {};
	bbd.boundingBoxVertices = m_BoundingBoxVerticesLocal;
	bbd.boundingBoxIndices = m_BoundingBoxIndicesLocal;

	bbc->AddBoundingBox(&bbd, &m_MeleeTransformTwo, L"sword");
}

