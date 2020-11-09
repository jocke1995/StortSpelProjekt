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
//#include ""

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
}

void UpgradeGUI::Update(double dt, Scene* scene)
{
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


	}
	// if the menu should not be showed AND we have not already deleted the menu, then delete the menu
	else if (m_Deleted == false)
	{


		m_Sm->RemoveEntity(scene->GetEntity("UpgradeMenuBackground"), scene);
		m_Sm->RemoveEntity(scene->GetEntity("UpgradeMenuDevider"), scene);
		for (int i = 0; i < m_ButtonNames.size(); i++)
		{
			if (scene->GetEntity(m_ButtonNames[i]))
			{
				m_Sm->RemoveEntity(scene->GetEntity(m_ButtonNames[i]), scene);
			}
		}
		m_ButtonNames.clear();
		m_Deleted = true;
		m_Drawn = false;


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
	textPos = { 0.49f, 0.16f };
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
	blended = { 1.0, 1.0, 1.0, 0.4 };
	notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		"UpgradeMenuBackground",
		quadPos, quadScale,
		false, false,
		1,
		blended,
		m_orangeGradientTexture);
	m_Sm->AddEntity(entity, scene);
	entity->Update(0);
	entity->SetEntityState(true);

	/* ------------------------- Upgrade Menu Background End --------------------------- */

	/* ------------------------- Upgrade Menu Devider --------------------------- */

	entity = scene->AddEntity("UpgradeMenuDevider");
	gui = entity->AddComponent<component::GUI2DComponent>();


	quadPos = { 0.45f, 0.20f };
	quadScale = { 0.5f, 0.001f };
	blended = { 1.0, 1.0, 1.0, 0.4 };
	notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		"UpgradeMenuDevider",
		quadPos, quadScale,
		false, false,
		1,
		blended,
		m_deviderTexture);
	m_Sm->AddEntity(entity, scene);
	entity->Update(0);
	entity->SetEntityState(true);

	/* ------------------------- Upgrade Menu Devider End --------------------------- */

	/* ------------------------- Upgrade Menu Buttons --------------------------- */
	int itterator = 0;
	for (auto u : m_AppliedUpgradeEnums)
	{
		makeUpgradeButton({ m_ButtonPos.x, (m_ButtonPos.y + (m_ButtonYOffset * itterator)) }, u.first);
		itterator++;
	}
	//makeUpgradeButton({ m_ButtonPos.x, m_ButtonPos.y }, "bbNo$");

	//makeUpgradeButton({ m_ButtonPos.x, (m_ButtonPos.y + m_ButtonYOffset) }, "bbMore$");
	/* ------------------------- Upgrade Menu Button End --------------------------- */

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

	EventBus::GetInstance().Subscribe(this, &UpgradeGUI::showMenu);
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

	// save the name so that we can use it for deletion and other things.
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

	quadPos = { pos.x, pos.y };//{ 0.47f, 0.202f };
	quadScale = { 0.2f, 0.04f };
	blended = { 1.0, 1.0, 1.0, 1.0 };
	notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		name,
		quadPos, quadScale,
		true, true,
		1,
		blended,
		m_buttonMintTexture);

	m_Sm->AddEntity(entity, m_CurrentScene);
	entity->Update(0);
	entity->SetEntityState(true);

}
