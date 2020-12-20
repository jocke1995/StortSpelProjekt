#include "stdafx.h"
#include "TextManager.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/Texture/Texture.h"
#include "../Renderer/DescriptorHeap.h"
#include "../Renderer/GPUMemory/ConstantBuffer.h"

#include "../Misc/Window.h"
#include "../Misc/AssetLoader.h"
#include "../Misc/GUI2DElements/Text.h"
#include "../Misc/GUI2DElements/Font.h"

#include "../Renderer/DX12Tasks/DX12Task.h"
#include "../Renderer/DX12Tasks/CopyOnDemandTask.h"

TextManager::TextManager()
{
	// Default text
	m_DefaultTextData.padding = { 0.5f, 0.0f };
	m_DefaultTextData.text = L"DEFAULT_TEXT";
}

TextManager::~TextManager()
{
	for (auto textMap : m_TextMap)
	{
		if (textMap.second != nullptr)
		{
			delete textMap.second;
		}
	}
	m_TextMap.clear();
}

const std::map<std::string, TextData>& TextManager::GetTextDataMap() const
{
	return m_TextDataMap;
}

TextData* TextManager::GetTextData(std::string name)
{
	return &m_TextDataMap[name];
}

void TextManager::AddText(std::string name)
{
	// Look if a font is chosen, otherwise, set the default font
	if (m_pFont == nullptr)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Font not set! Choosing default font...\n", name.c_str());
		m_pFont = AssetLoader::Get()->LoadFontFromFile(L"MedievalSharp.fnt");
	}

	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		Log::PrintSeverity(Log::Severity::WARNING, "It already exists a text with the name '%s'! Overwriting text data...\n", name.c_str());
	}

	m_TextDataMap.insert({ name, m_DefaultTextData });

	// Adding a new text to the map
	createText(name);

	// Create and Upload constantBuffer data
	Renderer* renderer = &Renderer::GetInstance();
	m_TextMap[name]->createCB(renderer->m_pDevice5, renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV]);
	submitCBTextDataToCodt(name);
}

void TextManager::SetFont(Font* font)
{
	if (m_pFont != nullptr)
	{
		delete m_pFont;
		m_pFont = nullptr;
	}

	m_pFont = font;
}

void TextManager::SetText(std::string text, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		m_TextDataMap[name].text = to_wstring(text);
		exists = true;
		replaceText(name);
		Renderer::GetInstance().submitTextToGPU(m_TextMap[name], this);
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set text.\n", name.c_str());
	}
}

void TextManager::SetPos(float2 textPos, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		m_TextMap[name]->updateCBbuffer("pos", { textPos.x, textPos.y, 0.0f, 0.0f });
		submitCBTextDataToCodt(name);
		exists = true;
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set position.\n", name.c_str());
	}
}

void TextManager::SetScale(float2 scale, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		// Scale with the size of the window
		Renderer* renderer = &Renderer::GetInstance();
		HWND* hwnd = const_cast<HWND*>(renderer->GetWindow()->GetHwnd());
		RECT rect;

		float win_x = 0, win_y = 0;
		if (GetWindowRect(*hwnd, &rect))
		{
			win_x = rect.right - rect.left;
			win_y = rect.bottom - rect.top;
		}

		float scale_x = 0, scale_y = 0;
		scale_x = win_x / 1000;
		scale_y = win_y / 1000;
		float aspect = scale_x / scale_y;

		float2 CBscale;
		CBscale.x = (scale.x * scale_x);
		CBscale.y = (scale.y * scale_y * aspect);

		m_TextMap[name]->updateCBbuffer("scale", { CBscale.x, CBscale.y, 0.0f, 0.0f });
		submitCBTextDataToCodt(name);
		exists = true;
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set scale.\n", name.c_str());
	}
}

void TextManager::SetPadding(float2 padding, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		m_TextDataMap[name].padding = padding;
		exists = true;
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set padding.\n", name.c_str());
	}
}

void TextManager::SetColor(float4 color, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		m_TextMap[name]->updateCBbuffer("color", color);
		submitCBTextDataToCodt(name);
		exists = true;
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set color.\n", name.c_str());
	}
}

void TextManager::SetBlend(float4 blend, std::string name)
{
	bool exists = false;
	auto it = m_TextDataMap.find(name);
	if (it != m_TextDataMap.end())
	{
		m_TextMap[name]->updateCBbuffer("blendFactor", blend);
		submitCBTextDataToCodt(name);
		exists = true;
	}

	if (exists == false)
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "The text '%s', does not exist! Could not set blend.\n", name.c_str());
	}
}

void TextManager::HideText(bool hide)
{
	m_TextIsHidden = hide;
}

const bool TextManager::IsTextHidden() const
{
	return m_TextIsHidden;
}

Font* TextManager::GetFont() const
{
	return m_pFont;
}

Texture* TextManager::GetFontTexture() const
{
	return m_pFont->GetTexture();
}

Text* TextManager::GetText(std::string name) const
{
	return m_TextMap.at(name);
}

std::map<std::string, Text*>* TextManager::GetTextMap()
{
	return &m_TextMap;
}

const int TextManager::GetNumOfTexts() const
{
	return m_TextMap.size();
}

const int TextManager::GetNumOfCharacters(std::string name) const
{
	return m_TextDataMap.at(name).text.size();
}

void TextManager::createText(std::string name)
{
	Renderer* renderer = &Renderer::GetInstance();
	TextData* textData = GetTextData(name);
	Text* text = new Text(
		renderer->m_pDevice5,
		renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV],
		m_pFont->GetTexture(),
		textData,
		m_pFont);

	m_TextMap.insert({ name, text });
}

void TextManager::replaceText(std::string name)
{
	Renderer* renderer = &Renderer::GetInstance();

	bool found = false;
	auto it = m_TextMap.find(name);
	if (it != m_TextMap.end())
	{
		Text* text = m_TextMap[name];

		// Set new data
		text->SetTextDataMap(&m_TextDataMap[name]);
		text->updateVertexData();
		text->updateCBbuffer();
		submitCBTextDataToCodt(name);

		// Remove old mesh data
		unsubmitText(name);

		// Init new mesh data
		text->initMeshData(renderer->m_pDevice5, renderer->m_DescriptorHeaps[DESCRIPTOR_HEAP_TYPE::CBV_UAV_SRV], m_pFont->GetTexture());
		found = true;
	}

	if (found == false)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Could not find any text called '%s' to replace!\n", name);
	}
}

void TextManager::submitCBTextDataToCodt(std::string name)
{
	if (m_TextMap[name]->m_pCB == nullptr)
	{
		return;
	}

	m_TextMap[name]->updateCBbuffer("hidden");

	// Submit to GPU
	const void* data = static_cast<const void*>(m_TextMap[name]->m_CBVec.data());

	Resource* uploadR = m_TextMap[name]->m_pCB->GetUploadResource();
	Resource* defaultR = m_TextMap[name]->m_pCB->GetDefaultResource();
	Renderer::GetInstance().submitToCodt(&std::make_tuple(uploadR, defaultR, data));
}

void TextManager::unsubmitText(std::string name)
{	  
	CopyTask* task = Renderer::GetInstance().m_CopyTasks[COPY_TASK_TYPE::COPY_ON_DEMAND];
	CopyOnDemandTask* codt = static_cast<CopyOnDemandTask*>(task);
	codt->UnSubmitText(m_TextMap[name]);
}
