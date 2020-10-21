#ifndef GUI2DCOMPONENT_H
#define GUI2DCOMPONENT_H

#include "Component.h"

#include "../Renderer/TextManager.h"
#include "../Renderer/QuadManager.h"

namespace component
{
	class GUI2DComponent : public Component
	{
	public:
		GUI2DComponent(Entity* parent);
		virtual ~GUI2DComponent();

		TextManager* GetTextManager();
		QuadManager* GetQuadManager();

		void Update(double dt);
		void OnInitScene();
		void OnUnInitScene();

	private:
		friend class Renderer;

		TextManager m_pTextMan;
		QuadManager m_pQuadMan;
	};
}
#endif
