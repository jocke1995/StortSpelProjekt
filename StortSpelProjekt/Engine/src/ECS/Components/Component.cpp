#include "stdafx.h"
#include "Component.h"
#include "../Renderer/Renderer.h"
#include "../Engine.h"

Component::Component(Entity* parent)
{
	m_pParent = parent;
}

Component::~Component()
{
}

void Component::Update(double dt)
{
}

void Component::RenderUpdate(double dt)
{
}

void Component::OnInitScene()
{
	Log::PrintSeverity(Log::Severity::WARNING, "COMPONENT InitScene called! Declare and define your own InitScene in your component class.\n");
}

void Component::OnLoadScene()
{
	Log::PrintSeverity(Log::Severity::WARNING, "COMPONENT OnLoadScene called! Declare and define your own OnLoadScene in your component class.\n");
}

void Component::OnUnloadScene()
{
	Log::PrintSeverity(Log::Severity::WARNING, "COMPONENT OnUnloadScene called! Declare and define your own OnUnloadScene in your component class.\n");
}

Entity* const Component::GetParent() const
{
	return m_pParent;
}
