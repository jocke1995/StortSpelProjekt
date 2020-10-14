#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include <map>
#include <vector>

class Window;
class Texture;
class Text;
class Font;

struct TextData;

class TextManager
{
public:
	TextManager();
	~TextManager();

	std::map<std::string, TextData>* const GetTextDataMap();
	TextData* GetTextData(std::string name);

	void AddText(std::string name);

	void SetFont(Font* font);
	void SetText(std::string text, std::string name);
	void SetPos(float2 textPos, std::string name);
	void SetScale(float2 scale, std::string name);
	void SetPadding(float2 padding, std::string name);
	void SetColor(float4 color, std::string name);

	Font* GetFont() const;
	Texture* GetFontTexture() const;
	Text* GetText(std::string name);
	std::map<std::string, Text*>* GetTextMap();
	const int GetNumOfTexts() const;
	const int GetNumOfCharacters(std::string name);

	void UploadTextData(std::string name);

private:
	std::map<std::string, TextData> m_TextDataMap = {};
	std::map<std::string, Text*> m_TextMap = {};
	Font* m_pFont = nullptr;

	void submitText(Text* text, std::string name);
	void replaceText(Text* text, std::string name);
};
#endif
