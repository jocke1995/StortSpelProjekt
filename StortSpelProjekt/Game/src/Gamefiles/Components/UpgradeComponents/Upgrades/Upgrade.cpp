#include "Upgrade.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

Upgrade::Upgrade(Entity* parentEntity)
{
	m_pParentEntity = parentEntity;
}

Upgrade::~Upgrade()
{
}

void Upgrade::SetParentEntity(Entity* parentEntity)
{
	m_pParentEntity = parentEntity;
}

void Upgrade::SetName(std::string name)
{
	m_Name = name;
}

std::string Upgrade::GetName() const
{
	return m_Name;
}

int Upgrade::GetLevel() const
{
	return m_Level;
}

void Upgrade::SetType(unsigned int type)
{
	m_Type = type;
}

unsigned int Upgrade::GetType() const
{
	return m_Type;
}

void Upgrade::SetID(int id)
{
	m_ID = id;
}

int Upgrade::GetID()
{
	return m_ID;
}

void Upgrade::Update(double dt)
{
}

void Upgrade::RenderUpdate(double dt)
{
}


void Upgrade::IncreaseLevel()
{
}

void Upgrade::OnHit()
{
}

void Upgrade::OnRangedHit()
{
}

void Upgrade::OnMeleeHit()
{
}

void Upgrade::OnDamage()
{
}

void Upgrade::OnPickUp()
{
}

void Upgrade::OnDeath()
{
}

void Upgrade::RangedFlight()
{
}

void Upgrade::RangedModifier()
{
}

void Upgrade::ApplyStat()
{
}
