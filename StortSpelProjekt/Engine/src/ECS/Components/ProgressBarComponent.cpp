#include "stdafx.h"
#include "ProgressBarComponent.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/GPUMemory/ConstantBuffer.h"
#include "../Renderer/Texture/Texture.h"

#include "../Misc/AssetLoader.h"

namespace component
{
	ProgressBarComponent::ProgressBarComponent(Entity* parent, float3 startPosition, float width, float height)
		:Component(parent)
	{
		m_Id = s_ProgressBarComponentCounter++;

		
		for (unsigned int i = 0; i < 2; i++)
		{
			m_QuadData[i].position = startPosition;
			m_QuadData[i].activePercent = 1.0f;
			m_QuadData[i].maxWidth = width;
			m_QuadData[i].maxHeight = height;
			m_QuadData[i].id = i;
		}
	}

	bool ProgressBarComponent::operator==(const ProgressBarComponent& other)
	{
		return m_Id == other.m_Id;
	}

	ProgressBarComponent::~ProgressBarComponent()
	{
		for (unsigned int i = 0; i < 2; i++)
		{
			if (m_ConstantBuffers[i] != nullptr)
			{
				delete m_ConstantBuffers[i];
			}
		}
	}

	void ProgressBarComponent::Update(double dt)
	{
		// TODO: Update
	}

	void ProgressBarComponent::OnInitScene()
	{
		Renderer::GetInstance().InitProgressBarComponent(this);
	}

	void ProgressBarComponent::OnUnInitScene()
	{
		Renderer::GetInstance().UnInitProgressBarComponent(this);
	}

	const float3 ProgressBarComponent::GetPosition() const
	{
		// The quads will have the same position, so it doesn't matter which one is returned
		return m_QuadData[0].position;
	}

	const float ProgressBarComponent::GetCurrentProgressBarPercent() const
	{
		// Only [1] will be the one that changes, since [0] is the background and will allways be at its full (1.0)
		return m_QuadData[1].activePercent;
	}

	const float ProgressBarComponent::GetMaxHeight() const
	{
		// The quads will have the same maxHeight, so it doesn't matter which one is returned
		return m_QuadData[0].maxHeight;
	}

	const float ProgressBarComponent::GetMaxWidth() const
	{
		// The quads will have the same maxWidth, so it doesn't matter which one is returned
		return m_QuadData[0].maxWidth;
	}

	// Updates position of both progress bars, since they should allways be at the same position
	void ProgressBarComponent::SetPosition(const float3& position)
	{
		for (unsigned int i = 0; i < 2; i++)
		{
			m_QuadData[i].position = position;
		}
	}

	void ProgressBarComponent::SetProgressBarPercent(float newProgressBarPercent)
	{
		if (newProgressBarPercent < 0)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Trying to set progress bar percent below zero\n");
			newProgressBarPercent = 0.0f;
		}
		else if (newProgressBarPercent > 1.0f)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Trying to set progress bar percent above one\n");
			newProgressBarPercent = 1.0f;
		}

		// Only [1] will update, since [0] is the background and should allways be at its full
		m_QuadData[1].activePercent =  newProgressBarPercent;
	}
	void ProgressBarComponent::SetTexture(PROGRESS_BAR_TYPE progressBarType, Texture* texture)
	{
		m_Textures[progressBarType] = texture;
	}
}
