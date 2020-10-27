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

		void HideText(bool hide);
		void HideQuad(bool hide);

		bool IsTextHidden() const;
		bool IsQuadHidden() const;

		void Update(double dt);
		void OnInitScene();
		void OnUnInitScene();

	private:
		friend class Renderer;

		TextManager m_pTextMan;
		QuadManager m_pQuadMan;

		// In case you want overlays or other possible moments
		// when it may be appropiate to hide the gui
		bool m_TextIsHidden = false;
		bool m_QuadIsHidden = false;
	};
}
#endif
