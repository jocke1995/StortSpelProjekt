#ifndef GUI2DCOMPONENT_H
#define GUI2DCOMPONENT_H

#include <map>
#include <vector>
#include "Component.h"
class Window;
class Texture;
class Text;
class Font;
class Mesh;
class Resource;
class ShaderResourceView;
struct TextData;
struct Vertex;

namespace component
{
	class GUI2DComponent : public Component
	{
	public:
		GUI2DComponent(Entity* parent);
		virtual ~GUI2DComponent();

		// Text functions
		std::map<std::string, TextData>* const GetTextDataMap();
		TextData* GetTextData(std::string name);

		void AddText(std::string name);
		void SubmitText(Text* text, std::string name);
		void ReplaceText(Text* text, std::string name);

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

		// Quad functions
		void CreateQuad(float2 pos, float2 scale, std::wstring path = L"NONE");

		// General functions
		void Update(double dt);
		void OnInitScene();
		void OnLoadScene();
		void OnUnloadScene();

	private:
		friend class Renderer;

		std::map<std::string, TextData> m_TextDataMap = {};
		std::map<std::string, Text*> m_TextMap = {};
		Font* m_pFont = nullptr;
		Mesh* m_pQuad = nullptr;
		Texture* m_pQuadTexture = nullptr;
	};
}
#endif
