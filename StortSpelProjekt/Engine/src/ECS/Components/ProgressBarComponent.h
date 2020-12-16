#ifndef PROGRESSBARCOMPONENT_H
#define PROGRESSBARCOMPONENT_H

#include "Component.h"
#include "structs.h"
#include <array>

#define PROGRESSBAR_TIME 3.0

class ConstantBuffer;
class Texture;

static unsigned int s_ProgressBarComponentCounter = 0;

enum PROGRESS_BAR_TYPE
{
	PROGRESS_BAR_TYPE_BACK,
	PROGRESS_BAR_TYPE_FRONT,
	NUM_PROGRESS_BAR_TYPES
};

namespace component
{
	class ProgressBarComponent : public Component
	{
	public:
		ProgressBarComponent(Entity* parent, float3 startPosition, float width, float height);
		bool operator==(const ProgressBarComponent& other);
		virtual ~ProgressBarComponent();

		void Update(double dt);
		void OnInitScene();
		void OnUnInitScene();

		// Gets
		const float3 GetPosition() const;
		const float GetCurrentProgressBarPercent() const;
		const float GetMaxHeight() const;
		const float GetMaxWidth() const;

		// Sets the position of the ProgressBar in the 3D-world. The position is the "middle of the quad"
		void SetPosition(const float3& position);
		// Value should be between 0-1, were 1 is FULL and 0 is empty
		void SetProgressBarPercent(const float newProgressBarPercent);
		// Sets the texture of the given type
		void SetTexture(PROGRESS_BAR_TYPE, Texture* texture);

		// Enable / Disable the drawing of the progressBar
		void EnableProgressBar();
		void DisableProgressBar();

	private:
		friend class Renderer;
		friend class ProgressBarRenderTask;

		// 0 will be overlay, 1 will be the actual progressBar
		std::array<PROGRESS_BAR_DATA, 2> m_QuadData;

		// Should be between 0-1
		float m_CurrentProgressBarPercent = 1.0f;

		unsigned int m_Id = 0;

		std::array<ConstantBuffer*, 2> m_ConstantBuffers;
		std::array<Texture*, 2> m_Textures;
		bool m_DrawState = true;

		double m_TimeElapsedSinceDrawedFirstTime = 0.0f;
	};
}

#endif
