#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include "Component.h"
class Text;
class Window;
class Texture;
struct TextData;
struct Font;

namespace component
{
	class TextComponent : public Component
	{
	public:
		TextComponent(Entity* parent, std::pair<Font*, Texture*> font);
		virtual ~TextComponent();

		std::vector<TextData>* const GetTextDataVec();

		void AddText();

		void SetFont(std::pair<Font*, Texture*> font);
		void SetText(std::string text, int pos);
		void SetPos(float2 textPos, int pos);
		void SetScale(float2 scale, int pos);
		void SetPadding(float2 padding, int pos);
		void SetColor(float4 color, int pos);

		Font* GetFont() const;
		Texture* GetTexture() const;
		const int GetNumOfCharacters(int pos) const;

		void Update(double dt);

	private:
		std::vector<TextData> m_TextDataVec;
		Font* m_pFont = nullptr;
		Texture* m_pFontTexture;
	};
}
#endif
