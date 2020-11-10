#include "UpgradeGUI.h"
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

UpgradeGUI::UpgradeGUI()
{

}

UpgradeGUI& UpgradeGUI::GetInstance()
{
	static UpgradeGUI instance;
	return instance;
}

UpgradeGUI::~UpgradeGUI()
{
	EventBus::GetInstance().Unsubscribe(this, &UpgradeGUI::showMenu);
	EventBus::GetInstance().Unsubscribe(this, &UpgradeGUI::getButtonPress);
}

void UpgradeGUI::Update(double dt, Scene* scene)
{
	// If we are not in game or shop scene then we should not show this menu. 
	// Thinking this might change depending on if this will be shown in the pause menu later.
	if (scene->GetName() != "GameScene" && scene->GetName() != "ShopScene")
	{
		m_Shown = false;
		m_Deleted = true;
	}

	if (m_CurrentScene != scene)
	{
		// Needs to be done here so these have the correct value when in a new scene.
		// E.g. the menu starts not shown.
		m_Shown = false;
		m_Drawn = false;
		m_Deleted = true;
		m_CurrentScene = scene;
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
				if (m_CurrentScene->EntityExists(m_ButtonNames[i]))
				{
					m_Sm->RemoveEntity(m_CurrentScene->GetEntity(m_ButtonNames[i]), m_CurrentScene);
				}
			}

			int itterator = 0;
			int posItterator = 0;

			//std::map<std::string, int> vec;
			//vec["TestButton1"] = 1;
			//vec["TestButton2"] = 2;
			//vec["TestButton3"] = 3;
			//vec["TestButton4"] = 4;
			//vec["TestButton5"] = 5;
			//vec["TestButton6"] = 6;
			//vec["TestButton7"] = 7;
			//vec["TestButton8"] = 8;
			//vec["TestButton9"] = 9;
			//vec["TestButton10"] = 10;
			//vec["TestButton11"] = 11;
			//vec["TestButton12"] = 12;
			//vec["TestButton13"] = 13;
			//vec["TestButton14"] = 14;
			//vec["TestButton15"] = 15;
			//vec["TestButton16"] = 16;
			//vec["TestButton17"] = 17;
			//vec["TestButton18"] = 18;
			//vec["TestButton19"] = 19;
			//vec["TestButton20"] = 20;
			//vec["TestButton21"] = 21;
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
				if (itterator >= 10 * m_TimesFilledMenu && itterator < 10 * (m_TimesFilledMenu + 1))
				{
					if (m_CurrentScene->EntityExists(u.first) == false)
					{
						makeUpgradeButton({ m_ButtonPos.x, (m_ButtonPos.y + (m_ButtonYOffset * posItterator)) }, u.first);
						posItterator++;
					}
				}
				itterator++;

			}
			m_TimesFilledMenu++;
			m_LoopButtons = false;
		}

	}
	// if the menu should not be showed AND we have not already deleted the menu, then delete the menu
	else if (m_Deleted == false)
	{
		// Delete the background
		m_Sm->RemoveEntity(scene->GetEntity("UpgradeMenuBackground"), scene);
		// If we have a next button it should be deleted
		if (m_ButtonsMultipleTen > 0)
		{
			m_Sm->RemoveEntity(scene->GetEntity("NextButton"), scene);
		}
		// If we have a description, that is not empty, it should be deleted
		if (m_CurrentDescription != "")
		{
			m_Sm->RemoveEntity(scene->GetEntity("Description"), scene);
			m_CurrentDescription = "";
		}
		// Delete the Upgrade Buttons
		for (int i = 0; i < m_ButtonNames.size(); i++)
		{
			if (scene->EntityExists(m_ButtonNames[i]))
			{
				m_Sm->RemoveEntity(scene->GetEntity(m_ButtonNames[i]), scene);
			}
		}
		m_ButtonNames.clear();
		m_Deleted = true;
		m_Drawn = false;
		m_ButtonsMultipleTen = 0;
		m_TimesFilledMenu = 0;
	}

}

void UpgradeGUI::CreateMenu(Scene* scene)
{
	m_CurrentScene = scene;
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

	/* ------------------------- Upgrade Menu Background --------------------------- */

	textToRender = "Bought Upgrades            Upgrade Description";
	textPos = { 0.49f, 0.165f };
	textPadding = { 0.5f, 0.0f };
	textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	textScale = { 0.5f, 0.5f };
	textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

	entity = scene->AddEntity("UpgradeMenuBackground");
	gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetTextManager()->AddText("UpgradeMenuBackground");
	gui->GetTextManager()->SetColor(textColor, "UpgradeMenuBackground");
	gui->GetTextManager()->SetPadding(textPadding, "UpgradeMenuBackground");
	gui->GetTextManager()->SetPos(textPos, "UpgradeMenuBackground");
	gui->GetTextManager()->SetScale(textScale, "UpgradeMenuBackground");
	gui->GetTextManager()->SetText(textToRender, "UpgradeMenuBackground");
	gui->GetTextManager()->SetBlend(textBlend, "UpgradeMenuBackground");

	quadPos = { 0.45f, 0.15f };
	quadScale = { 0.5f, 0.6f };
	blended = { 1.0, 1.0, 1.0, 1.0 };
	notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		"UpgradeMenuBackground",
		quadPos, quadScale,
		false, false,
		1,
		blended,
		m_boardBackgroundTexture, {0.4, 0.4, 0.4});
	m_Sm->AddEntity(entity, scene);
	entity->Update(0);
	entity->SetEntityState(true);

	/* ------------------------- Upgrade Menu Background End --------------------------- */

	/* ------------------------- Upgrade Menu Buttons --------------------------- */
	int itterator = 0;

	//std::map<std::string, int> vec;
	//vec["TestButton1"] = 1;
	//vec["TestButton2"] = 2;
	//vec["TestButton3"] = 3;
	//vec["TestButton4"] = 4;
	//vec["TestButton5"] = 5;
	//vec["TestButton6"] = 6;
	//vec["TestButton7"] = 7;
	//vec["TestButton8"] = 8;
	//vec["TestButton9"] = 9;
	//vec["TestButton10"] = 10;
	//vec["TestButton11"] = 11;
	//vec["TestButton12"] = 12;
	//vec["TestButton13"] = 13;
	//vec["TestButton14"] = 14;
	//vec["TestButton15"] = 15;
	//vec["TestButton16"] = 16;
	//vec["TestButton17"] = 17;
	//vec["TestButton18"] = 18;
	//vec["TestButton19"] = 19;
	//vec["TestButton20"] = 20;
	//vec["TestButton21"] = 21;

	for (auto u : m_AppliedUpgradeEnums)
	//for (auto u : vec)
	{
		if (itterator < 10)
		{
			makeUpgradeButton({ m_ButtonPos.x, (m_ButtonPos.y + (m_ButtonYOffset * itterator)) }, u.first);
		}

		if (itterator % 10 == 0 && itterator > 0)
		{
			m_ButtonsMultipleTen++;
		}
		itterator++;
	}
	m_TimesFilledMenu++;

	/* ------------------------- Upgrade Menu Button End --------------------------- */

	/* ------------------------- Upgrade Menu Next Button --------------------------- */

	if (m_ButtonsMultipleTen > 0)
	{
		textToRender = "Next";
		textPos = { 0.538f, m_ButtonPos.y + (m_ButtonYOffset * 10) };
		textPadding = { 0.5f, 0.0f };
		textColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		textScale = { 0.5f, 0.5f };
		textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

		entity = scene->AddEntity("NextButton");
		gui = entity->AddComponent<component::GUI2DComponent>();
		gui->GetTextManager()->AddText("NextButton");
		gui->GetTextManager()->SetColor(textColor, "NextButton");
		gui->GetTextManager()->SetPadding(textPadding, "NextButton");
		gui->GetTextManager()->SetPos(textPos, "NextButton");
		gui->GetTextManager()->SetScale(textScale, "NextButton");
		gui->GetTextManager()->SetText(textToRender, "NextButton");
		gui->GetTextManager()->SetBlend(textBlend, "NextButton");


		quadPos = { 0.49f, m_ButtonPos.y + (m_ButtonYOffset * 10) };//{ 0.47f, 0.202f };
		quadScale = { 0.15f, 0.04f };
		blended = { 1.0, 1.0, 1.0, 1.0 };
		notBlended = { 1.0, 1.0, 1.0, 1.0 };
		gui->GetQuadManager()->CreateQuad(
			"NextButton",
			quadPos, quadScale,
			true, true,
			1,
			blended,
			m_buttonParchment);

		m_Sm->AddEntity(entity, scene);
		entity->Update(0);
		entity->SetEntityState(true);
	}
	
	/* ------------------------- Upgrade Menu Next Button End --------------------------- */

	/* ----------------------- Upgrade Menu -------------------------- */

	// The menu is not deleted
	m_Deleted = false;
}

void UpgradeGUI::Init()
{
	AssetLoader* al = AssetLoader::Get();

	// Get textures
	m_deviderTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/black.png");
	m_buttonMintTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/rundadRekt2.png");
	m_buttonElipseTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/elipse.png");
	m_orangeBackgroundTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/orange.png");
	m_yellowGradientTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/YellowGradientBackground.png");
	m_orangeGradientTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/orangeGradient.png");
	m_boardBackgroundTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/board2.png");
	m_buttonParchment = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/parchment_hor.png");
	m_DescriptionParchment = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/parchment_vert.png");

	// Subscribe to events
	EventBus::GetInstance().Subscribe(this, &UpgradeGUI::showMenu);
	EventBus::GetInstance().Subscribe(this, &UpgradeGUI::getButtonPress);
}

void UpgradeGUI::SetCurrentScene(Scene* scene)
{
	m_CurrentScene = scene;
}

void UpgradeGUI::SetSceneMan(SceneManager* sceneManager)
{
	m_Sm = sceneManager;
}

void UpgradeGUI::SetShown(bool shown)
{
	m_Shown = shown;
}

void UpgradeGUI::showMenu(UForUpgrade* keyPress)
{
	m_Shown = !m_Shown;
}

void UpgradeGUI::makeUpgradeButton(float2 pos, std::string name)
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

	entity = m_CurrentScene->AddEntity(name);
	gui = entity->AddComponent<component::GUI2DComponent>();
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
		1,
		blended,
		m_buttonParchment);

	m_Sm->AddEntity(entity, m_CurrentScene);
	entity->Update(0);
	entity->SetEntityState(true);

}

void UpgradeGUI::getButtonPress(ButtonPressed* event)
{
	std::string upgradePartOfName = event->name.substr(0, 7);
	// Checking if the button that was pressed is an upgrade.
	if (upgradePartOfName == "Upgrade")
	{
		if (m_CurrentDescription != "")
		{
			m_Sm->RemoveEntity(m_CurrentScene->GetEntity("Description"), m_CurrentScene);
			m_CurrentDescription = "";
		}
		m_CurrentDescription = Player::GetInstance().GetUpgradeManager()->GetAllAvailableUpgrades().at(event->name)->GetDescription();

		updateDescription();
	}
	// Checking if the button is next. If so then set the bool to true so we change the buttons in update.
	else if (event->name == "NextButton")
	{
		m_LoopButtons = true;
	}
}

void UpgradeGUI::updateDescription()
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

	textPos = { 0.7 + 0.0065, m_ButtonPos.y + 0.03f };
	textPadding = { 0.5f, 0.0f };
	textColor = { .0f, .0f, .0f, 1.0f };
	textScale = { 0.215f, 0.215f };
	textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

	entity = m_CurrentScene->AddEntity(name);
	gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetTextManager()->AddText(name);
	gui->GetTextManager()->SetColor(textColor, name);
	gui->GetTextManager()->SetPadding(textPadding, name);
	gui->GetTextManager()->SetPos(textPos, name);
	gui->GetTextManager()->SetScale(textScale, name);
	gui->GetTextManager()->SetText(textToRender, name);
	gui->GetTextManager()->SetBlend(textBlend, name);

	quadPos = { 0.68, m_ButtonPos.y - 0.05f };
	quadScale = { 0.27f, 0.57f };
	blended = { 1.0, 1.0, 1.0, 1.0 };
	notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		name,
		quadPos, quadScale,
		true, false,
		1,
		blended,
		m_DescriptionParchment);

	m_Sm->AddEntity(entity, m_CurrentScene);
	entity->Update(0);
	entity->SetEntityState(true);
}
