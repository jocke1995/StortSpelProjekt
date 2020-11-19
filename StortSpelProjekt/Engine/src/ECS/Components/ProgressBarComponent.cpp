#include "stdafx.h"
#include "ProgressBarComponent.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/ProgressBar.h"

namespace component
{
	ProgressBarComponent::ProgressBarComponent(Entity* parent, float3 startPosition, float width, float height)
		:Component(parent)
	{
		for (unsigned int i = 0; i < 2; i++)
		{
			m_QuadData[i].position = startPosition;
			m_QuadData[i].activePercent = 1.0f - (i*0.5f);	// TODO: remove this code
			m_QuadData[i].maxWidth = width;
			m_QuadData[i].maxHeight = height;
		}
	}

	ProgressBarComponent::~ProgressBarComponent()
	{
		for (unsigned int i = 0; i < 2; i++)
		{
			if (m_ProgressBars[i] != nullptr)
			{
				delete m_ProgressBars[i];
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
		}
		else if (newProgressBarPercent > 1.0f)
		{
			Log::PrintSeverity(Log::Severity::WARNING, "Trying to set progress bar percent above one\n");
		}

		// Only [1] will update, since [0] is the background and should allways be at its full
		m_QuadData[1].activePercent =  newProgressBarPercent;
	}
}
