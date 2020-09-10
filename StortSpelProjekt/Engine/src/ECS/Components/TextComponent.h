#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include "Component.h"
#include "../Renderer/Text.h"
class Text;
class Window;

struct TextData
{
	std::wstring text;
	float2 pos;
	float2 scale;
	float2 padding;
	float4 color;
	Font font;
};

namespace component
{
	class TextComponent : public Component
	{
	public:
		TextComponent(Entity* parent);
		virtual ~TextComponent();

		//void SetText(Text* text, int pos);
		std::vector<TextData*> const GetTextDataVec() const;

		void AddText();

		void SetFont(Window* window, std::string fontPath, int pos);
		void SetText(std::string text, int pos);
		void SetPos(float2 textPos, int pos);
		void SetScale(float2 scale, int pos);
		void SetPadding(float2 padding, int pos);
		void SetColor(float4 color, int pos);

		void Update(double dt);

	private:
		std::vector<TextData*> m_TextDataVec;
	};
}
#endif
