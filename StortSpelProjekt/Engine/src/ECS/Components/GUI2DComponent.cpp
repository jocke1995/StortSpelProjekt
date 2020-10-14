#include "stdafx.h"
#include <vector>
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
		m_pTextMan = new TextManager();
		m_pQuadMan = new QuadManager();
	}

	GUI2DComponent::~GUI2DComponent()
	{
		delete m_pTextMan;
		delete m_pQuadMan;
	}

	TextManager* const GUI2DComponent::GetTextManager() const
	{
		return m_pTextMan;
	}

	QuadManager* const GUI2DComponent::GetQuadManager() const
	{
		return m_pQuadMan;
	}

	void GUI2DComponent::Update(double dt)
	{
	}

	void GUI2DComponent::OnInitScene()
	{
		Renderer::GetInstance().InitGUI2DComponent(GetParent());
	}

	void GUI2DComponent::OnLoadScene()
	{
		Renderer::GetInstance().LoadTexture(m_pTextMan->GetFont()->GetTexture());
		Renderer::GetInstance().LoadTexture(m_pQuadMan->GetTexture());
	}

	void GUI2DComponent::OnUnloadScene()
	{
		Renderer::GetInstance().UnloadTexture(m_pTextMan->GetFont()->GetTexture());
		Renderer::GetInstance().LoadTexture(m_pQuadMan->GetTexture());
	}
}
