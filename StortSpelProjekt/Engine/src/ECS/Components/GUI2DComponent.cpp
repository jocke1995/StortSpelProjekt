#include "stdafx.h"
#include "GUI2DComponent.h"

#include "../Renderer/TextManager.h"
#include "../Renderer/QuadManager.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Texture/Texture.h"

#include "../Misc/GUI2DElements/Font.h"

namespace component
{
	GUI2DComponent::GUI2DComponent(Entity* parent)
		:Component(parent)
	{
	}

	GUI2DComponent::~GUI2DComponent()
	{
	}

	TextManager* GUI2DComponent::GetTextManager()
	{
		return &m_pTextMan;
	}

	QuadManager* GUI2DComponent::GetQuadManager()
	{
		return &m_pQuadMan;
	}

	void GUI2DComponent::Update(double dt)
	{
	}

	void GUI2DComponent::OnInitScene()
	{
		Renderer::GetInstance().InitGUI2DComponent(GetParent());
	}
}
