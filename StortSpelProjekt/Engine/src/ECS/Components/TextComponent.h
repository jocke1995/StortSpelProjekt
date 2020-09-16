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

		std::map<std::string, TextData>* const GetTextDataMap();

		void AddText(std::string name);
		void SubmitText(Text* text);

		void SetFont(std::pair<Font*, Texture*> font);
		void SetText(std::string text, std::string name);
		void SetPos(float2 textPos, std::string name);
		void SetScale(float2 scale, std::string name);
		void SetPadding(float2 padding, std::string name);
		void SetColor(float4 color, std::string name);

		Font* GetFont() const;
		Texture* GetTexture() const;
		Text* GetText(int pos) const;
		const int GetNumOfTexts() const;
		const int GetNumOfCharacters(std::string name);

		void Update(double dt);

	private:
		std::map<std::string, TextData> m_TextDataMap = {};
		std::vector<Text*> m_TextVec = {};
		Font* m_pFont = nullptr;
		Texture* m_pFontTexture = nullptr;
	};
}
#endif
