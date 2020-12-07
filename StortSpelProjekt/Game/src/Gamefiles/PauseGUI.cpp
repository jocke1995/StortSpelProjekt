#include "PauseGUI.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "ECS/Components/GUI2DComponent.h"
#include "ECS/Scene.h"
#include "ECS/SceneManager.h"
#include "Player.h"
#include "Misc/AssetLoader.h"
#include "Events/EventBus.h"
#include "Renderer/Texture/Texture.h"
#include "Renderer/Renderer.h"
#include "UpgradeManager.h"
#include "Components/UpgradeComponents/UpgradeComponent.h"
#include "Misc/GUI2DElements/Font.h"

PauseGUI::PauseGUI()
{

}

PauseGUI& PauseGUI::GetInstance()
{
	static PauseGUI instance;
	return instance;
}

PauseGUI::~PauseGUI()
{
	EventBus::GetInstance().Unsubscribe(this, &PauseGUI::showMenu);
	EventBus::GetInstance().Unsubscribe(this, &PauseGUI::getButtonPress);
}

void PauseGUI::Update(double dt, Scene* scene)
{
	// If we are not in game or shop scene then we should not show this menu. 
	// Thinking this might change depending on if this will be shown in the pause menu later.
	if (scene->GetName() != "GameScene" && scene->GetName() != "ShopScene")
	{
		m_Shown = false;
		m_Deleted = true;
	}

	if (m_pCurrentScene != scene)
	{
		// Needs to be done here so these have the correct value when in a new scene.
		// E.g. the menu starts not shown.
		m_Shown = false;
		m_Drawn = false;
		m_Deleted = true;
		m_pCurrentScene = scene;
		m_CurrentDescription = "";
		m_ButtonsMultipleTen = 0;
		m_TimesFilledMenu = 0;
	}

	// if shown is true we should draw the menu
	if (m_Shown == true)
	{

		// if current scene is the same as the saved scene then we don't have do out anything. 
		// Since it should be done already.
		if (m_Drawn == false)
		{
			// Will draw the menu and set current scene and set deleted to false.
			CreateMenu(scene);
			m_Drawn = true;
		}

		// Here we change the Upgrade buttons if we have pressed the next button.
		// Next button is only shown when we have more than 10 object so we acually need to cycle between them.
		if (m_LoopButtons)
		{
			// Delete existing buttons.
			for (int i = 0; i < m_ButtonNames.size(); i++)
			{
				if (m_pCurrentScene->EntityExists(m_ButtonNames[i]))
				{
					m_pSm->RemoveEntity(m_pCurrentScene->GetEntity(m_ButtonNames[i]), m_pCurrentScene);
				}
			}

			int iterator = 0;
			int posIterator = 0;

			// Loop through and populate with the next buttons that should be shown.
			if (m_TimesFilledMenu > m_ButtonsMultipleTen)
			{
				m_TimesFilledMenu = 0;
			}
			for (auto u : m_AppliedUpgradeEnums)
			//for (auto u : vec)
			{
				// Bigger than number of times the menu has been filled multiplied by the number of buttons, which is 10.
				// But we want no more than 10 buttons at a time so do this no more than 10 times 
				// example: i >= 10 * 1 && i < 10 * 2. So we add buttons at place 10 to 19
				if (iterator >= 10 * m_TimesFilledMenu && iterator < 10 * (m_TimesFilledMenu + 1))
				{
					if (m_pCurrentScene->EntityExists(u.first) == false)
					{
						makeUpgradeButton({ m_ButtonPos.x, (m_ButtonPos.y + (m_ButtonYOffset * posIterator)) }, u.first);
						posIterator++;
					}
				}
				iterator++;

			}
			m_TimesFilledMenu++;
			m_LoopButtons = false;
		}

	}
	// if the menu should not be showed AND we have not already deleted the menu, then delete the menu
	else if (m_Deleted == false)
	{
		// Delete the background
		m_pSm->RemoveEntity(scene->GetEntity("UpgradeMenuBackground"), scene);
		// If we have a next button it should be deleted
		if (m_ButtonsMultipleTen > 0)
		{
			m_pSm->RemoveEntity(scene->GetEntity("NextButton"), scene);
		}
		// If we have a description, that is not empty, it should be deleted
		if (m_CurrentDescription != "")
		{
			m_pSm->RemoveEntity(scene->GetEntity("Description"), scene);
			m_CurrentDescription = "";
		}

		m_pSm->RemoveEntity(scene->GetEntity("PauseOverlay"), scene);
		// Delete the Upgrade Buttons
		for (int i = 0; i < m_ButtonNames.size(); i++)
		{
			if (scene->EntityExists(m_ButtonNames[i]))
			{
				m_pSm->RemoveEntity(scene->GetEntity(m_ButtonNames[i]), scene);
			}
		}
		m_ButtonNames.clear();
		m_Deleted = true;
		m_Drawn = false;
		m_ButtonsMultipleTen = 0;
		m_TimesFilledMenu = 0;
	}

}

void PauseGUI::CreateMenu(Scene* scene)
{
	m_pCurrentScene = scene;
	// Get this so we know which upgrades the player has.
	m_AppliedUpgradeEnums = Player::GetInstance().GetUpgradeManager()->GetAppliedUpgradeEnums();

	Entity* entity = nullptr;
	component::GUI2DComponent* gui = nullptr;
	std::string textToRender;
	float2 textPos;
	float2 textPadding;
	float4 textColor;
	float2 textScale;
	float4 textBlend;

	float2 quadPos;
	float2 quadScale;
	float4 blended;
	float4 notBlended;
	/* ------------------------- Upgrade Menu --------------------------- */

	entity = scene->AddEntity("PauseOverlay");
	if (entity != nullptr)
	{
		gui = entity->AddComponent<component::GUI2DComponent>();
		quadPos = { 0.0f, 0.0f };
		quadScale = { 120.0f, 67.5f };
		blended = { 1.0, 1.0, 1.0, 1.0 };
		notBlended = { 1.0, 1.0, 1.0, 1.0 };
		gui->GetQuadManager()->CreateQuad(
			"PauseOverlay",
			quadPos, quadScale,
			false, false,
			3,
			notBlended,
			m_pPauseOverlayTexture, { 0.4, 0.4, 0.4 });
		scene->InitDynamicEntity(entity);
		entity->Update(0);
	}

	/* ------------------------- Upgrade Menu Background --------------------------- */

	textToRender = "Bought Upgrades            Upgrade Description";
	textPos = { 0.55f, 0.095f };
	textPadding = { 0.5f, 0.0f };
	textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	textScale = { 0.5f, 0.5f };
	textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

	entity = scene->AddEntity("UpgradeMenuBackground");
	// This is not a solution that is good... The core issue is not fixed
	if (entity != nullptr)
	{
		gui = entity->AddComponent<component::GUI2DComponent>();
		gui->GetTextManager()->SetFont(m_pArial);
		gui->GetTextManager()->AddText("UpgradeMenuBackground");
		gui->GetTextManager()->SetColor(textColor, "UpgradeMenuBackground");
		gui->GetTextManager()->SetPadding(textPadding, "UpgradeMenuBackground");
		gui->GetTextManager()->SetPos(textPos, "UpgradeMenuBackground");
		gui->GetTextManager()->SetScale(textScale, "UpgradeMenuBackground");
		gui->GetTextManager()->SetText(textToRender, "UpgradeMenuBackground");
		gui->GetTextManager()->SetBlend(textBlend, "UpgradeMenuBackground");

		quadPos = { 0.51f, 0.08f };
		quadScale = { 0.5f, 0.6f };
		blended = { 1.0, 1.0, 1.0, 1.0 };
		notBlended = { 1.0, 1.0, 1.0, 1.0 };
		gui->GetQuadManager()->CreateQuad(
			"UpgradeMenuBackground",
			quadPos, quadScale,
			false, false,
			4,
			notBlended,
			m_pBoardBackgroundTexture, { 0.4, 0.4, 0.4 });
		scene->InitDynamicEntity(entity);
		entity->Update(0);

		/* ------------------------- Upgrade Menu Background End --------------------------- */

		/* ------------------------- Upgrade Menu Buttons --------------------------- */
		int iterator = 0;

		for (auto u : m_AppliedUpgradeEnums)
			//for (auto u : vec)
		{
			if (iterator < 10)
			{
				makeUpgradeButton({ m_ButtonPos.x, (m_ButtonPos.y + (m_ButtonYOffset * iterator)) }, u.first);
			}

			if (iterator % 10 == 0 && iterator > 0)
			{
				m_ButtonsMultipleTen++;
			}
			iterator++;
		}
		m_TimesFilledMenu++;

		/* ------------------------- Upgrade Menu Button End --------------------------- */

		/* ------------------------- Upgrade Menu Next Button --------------------------- */

		if (m_ButtonsMultipleTen > 0)
		{
			textToRender = "Next";
			textPos = { m_ButtonPos.x + 0.025f, m_ButtonPos.y + (m_ButtonYOffset * 10) };
			textPadding = { 0.5f, 0.0f };
			textColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			textScale = { 0.5f, 0.5f };
			textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

			entity = scene->AddEntity("NextButton");
			gui = entity->AddComponent<component::GUI2DComponent>();
			gui->GetTextManager()->SetFont(m_pArial);
			gui->GetTextManager()->AddText("NextButton");
			gui->GetTextManager()->SetColor(textColor, "NextButton");
			gui->GetTextManager()->SetPadding(textPadding, "NextButton");
			gui->GetTextManager()->SetPos(textPos, "NextButton");
			gui->GetTextManager()->SetScale(textScale, "NextButton");
			gui->GetTextManager()->SetText(textToRender, "NextButton");
			gui->GetTextManager()->SetBlend(textBlend, "NextButton");


			quadPos = { m_ButtonPos.x, m_ButtonPos.y + (m_ButtonYOffset * 10) };//{ 0.47f, 0.202f };
			quadScale = { 0.15f, 0.04f };
			blended = { 1.0, 1.0, 1.0, 1.0 };
			notBlended = { 1.0, 1.0, 1.0, 1.0 };
			gui->GetQuadManager()->CreateQuad(
				"NextButton",
				quadPos, quadScale,
				true, true,
				4,
				notBlended,
				m_pButtonParchment);

			scene->InitDynamicEntity(entity);
			entity->Update(0);
		}

		/* ------------------------- Upgrade Menu Next Button End --------------------------- */

		/* ----------------------- Upgrade Menu -------------------------- */

		// The menu is not deleted
		m_Deleted = false;
	}
	
}

void PauseGUI::Init()
{
	AssetLoader* al = AssetLoader::Get();

	// Get textures
	m_pBoardBackgroundTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/board2.png");
	m_pButtonParchment = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/parchment_hor.png");
	m_pDescriptionParchment = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/parchment_vert.png");
	m_pPauseOverlayTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/PauseOverlay.png");

	m_pArial = al->LoadFontFromFile(L"Arial.fnt");

	// Subscribe to events
	EventBus::GetInstance().Subscribe(this, &PauseGUI::showMenu);
	EventBus::GetInstance().Subscribe(this, &PauseGUI::getButtonPress);
}

void PauseGUI::SetCurrentScene(Scene* scene)
{
	m_pCurrentScene = scene;
}

void PauseGUI::SetSceneMan(SceneManager* sceneManager)
{
	m_pSm = sceneManager;
}

void PauseGUI::SetShown(bool shown)
{
	m_Shown = shown;
}

void PauseGUI::showMenu(PauseGame* keyPress)
{
	m_Shown = !m_Shown;
}

void PauseGUI::makeUpgradeButton(float2 pos, std::string name)
{
	Entity* entity = nullptr;
	component::GUI2DComponent* gui = nullptr;
	std::string textToRender;
	float2 textPos;
	float2 textPadding;
	float4 textColor;
	float2 textScale;
	float4 textBlend;

	float2 quadPos;
	float2 quadScale;
	float4 blended;
	float4 notBlended;

	// Save the name so that we can use it for deletion and other things.
	m_ButtonNames.push_back(name);

	std::string str = name;
	// Used to skipp the "Upgrade" part of the name
	textToRender = str.substr(7);
	textPos = { pos.x + 0.025f, pos.y};//{ 0.495f, 0.202f };
	textPadding = { 0.5f, 0.0f };
	textColor = { .0f, .0f, .0f, 1.0f };
	textScale = { 0.5f, 0.5f };
	textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

	entity = m_pCurrentScene->AddEntity(name);
	gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetTextManager()->SetFont(m_pArial);
	gui->GetTextManager()->AddText(name);
	gui->GetTextManager()->SetColor(textColor, name);
	gui->GetTextManager()->SetPadding(textPadding, name);
	gui->GetTextManager()->SetPos(textPos, name);
	gui->GetTextManager()->SetScale(textScale, name);
	gui->GetTextManager()->SetText(textToRender, name);
	gui->GetTextManager()->SetBlend(textBlend, name);

	quadPos = { pos.x, pos.y };
	quadScale = { 0.2f, 0.04f };
	blended = { 1.0, 1.0, 1.0, 1.0 };
	notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		name,
		quadPos, quadScale,
		true, true,
		4,
		notBlended,
		m_pButtonParchment);

	m_pCurrentScene->InitDynamicEntity(entity);
	entity->Update(0);

}

void PauseGUI::getButtonPress(ButtonPressed* event)
{
	std::string upgradePartOfName = event->name.substr(0, 7);
	// Checking if the button that was pressed is an upgrade.
	if (upgradePartOfName == "Upgrade")
	{
		if (m_CurrentDescription != "")
		{
			m_pSm->RemoveEntity(m_pCurrentScene->GetEntity("Description"), m_pCurrentScene);
			m_CurrentDescription = "";
		}
		Upgrade* upgrade = Player::GetInstance().GetUpgradeManager()->GetAllAvailableUpgrades().at(event->name);
		m_CurrentDescription = upgrade->GetDescription(upgrade->GetLevel());
		
		updateDescription(upgrade->GetLevel());
	}
	// Checking if the button is next. If so then set the bool to true so we change the buttons in update.
	else if (event->name == "NextButton")
	{
		m_LoopButtons = true;
	}
}

void PauseGUI::updateDescription(int level)
{
	Entity* entity = nullptr;
	component::GUI2DComponent* gui = nullptr;
	std::string textToRender;
	float2 textPos;
	float2 textPadding;
	float4 textColor;
	float2 textScale;
	float4 textBlend;

	float2 quadPos;
	float2 quadScale;
	float4 blended;
	float4 notBlended;

	std::string name = "Description";
	int newLinePos = 0;
	int sizeLeft = m_CurrentDescription.size();
	std::string description = m_CurrentDescription;

	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;
	int newLineAmount = 1;
	// Loop through the description and save every word to the sting that will be drawn on screen.
	// If we reach a character length of 50 then put in a newLine character after the word to split the scentence appropriately.
	while ((pos = description.find(delimiter)) != std::string::npos) 
	{
		token = description.substr(0, pos);
		newLinePos += pos;
		// If we exceed 42 characters in lenght then we put a newLine before the word that broke the limit.
		if (newLinePos > (42 * newLineAmount))
		{
			newLineAmount++;
			textToRender += "\n" + token + " ";
		}
		// Else we just take the word as is and put back a space
		else
		{
			textToRender += token + " ";
		}
		// Get the last word that we didn't get in the loop
		description.erase(0, pos + delimiter.length());
	}
	textToRender += description.substr(0, description.length());
	textToRender += "\n\nCurrent level: " + std::to_string(level);

	textPos = { 0.76 + 0.0065, m_ButtonPos.y + 0.03f };
	textPadding = { 0.5f, 0.0f };
	textColor = { .0f, .0f, .0f, 1.0f };
	textScale = { 0.215f, 0.215f };
	textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

	entity = m_pCurrentScene->AddEntity(name);
	gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetTextManager()->SetFont(m_pArial);
	gui->GetTextManager()->AddText(name);
	gui->GetTextManager()->SetColor(textColor, name);
	gui->GetTextManager()->SetPadding(textPadding, name);
	gui->GetTextManager()->SetPos(textPos, name);
	gui->GetTextManager()->SetScale(textScale, name);
	gui->GetTextManager()->SetText(textToRender, name);
	gui->GetTextManager()->SetBlend(textBlend, name);

	quadPos = { 0.74, m_ButtonPos.y - 0.05f };
	quadScale = { 0.27f, 0.57f };
	blended = { 1.0, 1.0, 1.0, 1.0 };
	notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		name,
		quadPos, quadScale,
		true, false,
		4,
		notBlended,
		m_pDescriptionParchment);

	m_pCurrentScene->InitDynamicEntity(entity);
	entity->Update(0);
}
