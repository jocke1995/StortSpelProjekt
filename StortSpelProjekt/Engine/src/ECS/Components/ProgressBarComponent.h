#ifndef PROGRESSBARCOMPONENT_H
#define PROGRESSBARCOMPONENT_H

#include "Component.h"
#include "structs.h"
#include <array>

class ProgressBar;

namespace component
{
	class ProgressBarComponent : public Component
	{
	public:
		ProgressBarComponent(Entity* parent, float3 startPosition, float width, float height);
		virtual ~ProgressBarComponent();

		void Update(double dt);
		void OnInitScene();
		void OnUnInitScene();

		// Gets
		const float3 GetPosition() const;

		const float GetMaxHeight() const;
		const float GetMaxWidth() const;

		// Sets the position of the ProgressBar in the 3D-world. The position is the "middle of the quad"
		void SetPosition(const float3& position);
		// Value should be between 0-1, were 1 is FULL and 0 is empty
		void SetProgressBarPercent(const float newProgressBarPercent);

	private:
		friend class Renderer;
		std::array<ProgressBar*, 2> m_ProgressBars;


		// 0 will be overlay, 1 will be the actual progressBar
		std::array<PROGRESS_BAR_DATA, 2> m_QuadData;

		// Should be between 0-1
		float m_CurrentProgressBarPercent = 1.0f;
	};
}


#endif