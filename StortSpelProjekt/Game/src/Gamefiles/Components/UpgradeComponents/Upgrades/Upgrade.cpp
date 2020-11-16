#include "Upgrade.h"
#include "EngineMath.h"
#include "ECS/Entity.h"

Upgrade::Upgrade(Entity* parentEntity)
{
	m_pParentEntity = parentEntity;
	m_ImageName = "default.png";
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

void Upgrade::SetImage(std::string* imageName)
{ 
	m_ImageName = *imageName;
}

std::string Upgrade::GetImage() const
{
	return m_ImageName;
}

int Upgrade::GetLevel() const
{
	return m_Level;
}

int Upgrade::GetMaxLevel() const
{
	return m_MaxLevel;
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

std::string Upgrade::GetDescription(unsigned int level)
{
	return "THIS UPGRADE IS MISSING A DESCRIPTION";
}

int Upgrade::GetPrice()
{
	return m_Price;
}

void Upgrade::Update(double dt)
{
}

void Upgrade::RenderUpdate(double dt)
{
}

void Upgrade::SetLevel(unsigned int lvl)
{
	m_Level = lvl;
}


void Upgrade::IncreaseLevel()
{
}

void Upgrade::IncreaseLevelOnly()
{
	m_Level++;
}

void Upgrade::OnHit()
void Upgrade::OnHit(Entity* target)
{
}

void Upgrade::OnRangedHit(Entity* target)
{
}

void Upgrade::OnMeleeHit(Entity* target)
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

void Upgrade::ApplyBoughtUpgrade()
{
}
