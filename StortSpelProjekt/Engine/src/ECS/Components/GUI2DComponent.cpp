#include "stdafx.h"
#include "GUI2DComponent.h"

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
		if (m_pQuadMan.IsMarkable() && !m_pQuadMan.IsQuadHidden())
		{
			if (m_pQuadMan.IsMarked())
			{
				m_pQuadMan.SetActiveTexture(1);
			}
			else
			{
				m_pQuadMan.SetActiveTexture(0);
			}
		}
	}

	void GUI2DComponent::OnInitScene()
	{
		Renderer::GetInstance().InitGUI2DComponent(this);
	}

	void GUI2DComponent::OnUnInitScene()
	{
		Renderer::GetInstance().UnInitGUI2DComponent(this);
	}
}
