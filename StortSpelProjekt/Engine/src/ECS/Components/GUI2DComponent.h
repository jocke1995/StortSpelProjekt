#ifndef GUI2DCOMPONENT_H
#define GUI2DCOMPONENT_H

#include <map>
#include <vector>
#include "Component.h"

class TextManager;
class QuadManager;

namespace component
{
	class GUI2DComponent : public Component
	{
	public:
		GUI2DComponent(Entity* parent);
		virtual ~GUI2DComponent();

		TextManager* const GetTextManager() const;
		QuadManager* const GetQuadManager() const;

		void Update(double dt);
		void OnInitScene();
		void OnLoadScene();
		void OnUnloadScene();

	private:
		friend class Renderer;

		TextManager* m_pTextMan = nullptr;
		QuadManager* m_pQuadMan = nullptr;
	};
}
#endif
