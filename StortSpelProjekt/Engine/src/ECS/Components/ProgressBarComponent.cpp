#include "stdafx.h"
#include "ProgressBarComponent.h"

namespace component
{
	ProgressBarComponent::ProgressBarComponent(Entity* parent, float3 startPosition, float width, float height)
		:Component(parent)
	{
		for (unsigned int i = 0; i < 2; i++)
		{
			m_QuadData[i].position = startPosition;
			m_QuadData[i].activePercent = 1.0f;
			m_QuadData[i].maxWidth = width;
			m_QuadData[i].maxHeight = height;
		}
	}

	ProgressBarComponent::~ProgressBarComponent()
	{
	}

	void ProgressBarComponent::Update(double dt)
	{
	}

	void ProgressBarComponent::OnInitScene()
	{
	}

	void ProgressBarComponent::OnUnInitScene()
	{
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
		// Only [1] will update, since [0] is the background and should allways be at its full
		m_QuadData[1].activePercent =  newProgressBarPercent;
	}
}