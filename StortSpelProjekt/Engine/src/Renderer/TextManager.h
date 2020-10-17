#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include <map>
#include <vector>
#include "../Misc/GUI2DElements/Text.h"

class Window;
class Texture;
class Text;
class Font;

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
	Text* GetText(std::string name) const;
	std::map<std::string, Text*>* GetTextMap();
	const int GetNumOfTexts() const;
	const int GetNumOfCharacters(std::string name) const;

	void UploadAndExecuteTextData(std::string name);

private:
	friend class Renderer;

	std::map<std::string, TextData> m_TextDataMap = {};
	std::map<std::string, Text*> m_TextMap = {};
	Font* m_pFont = nullptr;
	TextData m_DefaultTextData;

	void submitText(Text* text, std::string name);
	void replaceText(Text* text, std::string name);
	void uploadTextData(std::string name, Renderer* renderer);
};
#endif
