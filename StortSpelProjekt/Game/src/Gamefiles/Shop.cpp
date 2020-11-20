#include "Shop.h"
#include <time.h>
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Components/UpgradeComponents/UpgradeComponent.h"
#include "UpgradeManager.h"
#include "Components/CurrencyComponent.h"
#include "Misc/AssetLoader.h"
#include "Renderer/Texture/Texture.h"
#include "Events/EventBus.h"
#include "ECS/SceneManager.h"
#include "Misc/GUI2DElements/Font.h"

#include "Player.h"

#include "Renderer/Renderer.h"

Shop::Shop()
{
	m_pUpgradeManager = Player::GetInstance().GetUpgradeManager();
	m_pPlayer = Player::GetInstance().GetPlayer();
	m_AllAvailableUpgrades = m_pUpgradeManager->GetAllAvailableUpgrades();
	for (auto upgrade : m_AllAvailableUpgrades)
	{
		m_UpgradeNames.push_back(upgrade.first);
	}
	m_Rand = EngineRand(time(NULL));
	// Set the size of shop inventory - how many upgrades the shop will contain.
	m_InvSize = 3;
	m_RerollCost = 50;
	m_RerollIncrease = m_RerollCost / 10; // 1/10 of the base cost is increased each time the player uses the reroll function. 


	AssetLoader* al = AssetLoader::Get();
	m_pArial = al->LoadFontFromFile(L"Arial.fnt");

	EventBus::GetInstance().Subscribe(this, &Shop::shopButtonPressed);
	EventBus::GetInstance().Subscribe(this, &Shop::sceneChange);

	EventBus::GetInstance().Subscribe(this, &Shop::OnShopGUIStateChange);
}

Shop::~Shop()
{
	EventBus::GetInstance().Unsubscribe(this, &Shop::OnShopGUIStateChange);
}

void Shop::Create2DGUI()
{
	// The 2DGUI is already active, dont create it again
	if (m_DisplayingShopGUI == true)
	{
		//Log::PrintSeverity(Log::Severity::WARNING, "Trying to Create Shop2D-GUI when it already exists!\n");
		return;
	}

	
	component::GUI2DComponent* gui = nullptr;
	SceneManager& sm = SceneManager::GetInstance();
	Scene* shopScene = sm.GetScene("ShopScene");

	/* ---------------------------Background----------------------------- */
	Entity* entity = shopScene->AddEntity("background");
	gui = entity->AddComponent<component::GUI2DComponent>();
	float2 quadPos = { 0.0f, 0.1f };
	float2 quadScale = { 1.0f, 0.8f };
	float4 blended = { 1.0, 1.0, 1.0, 0.75 };
	float4 notBlended = { 1.0, 1.0, 1.0, 1.0 };
	Texture* background = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/board2.png");
	gui->GetQuadManager()->CreateQuad(
		"background",
		quadPos, quadScale,
		false, false,
		1,
		notBlended,
		background
	);
	entity->SetEntityState(true);	// true == dynamic, which means it will be removed when a new scene is set
	sm.AddEntity(entity, shopScene);

	/* ------------------------- Shop Buttons --------------------------- */
	for (int i = 0; i < GetInventorySize(); i++)
	{
		Upgrade* upgrade = m_AllAvailableUpgrades.find(m_InventoryNames.at(i))->second;

		// Bought Text on the buttons.
		std::string textToRender = s_UpgradeBoughtText;
		if (m_InventoryIsBought.at(i) == false)
		{
			// If the upgrade isn't bought, write the description on the button
			textToRender = upgrade->GetDescription(upgrade->GetLevel() + 1);
			textToRender += "\nPrice: " + std::to_string(GetPrice(GetInventoryNames().at(i)));
			textToRender += "    Next Level: " + std::to_string(upgrade->GetLevel() + 1);
		}
		
		float2 textPos = { 0.17f, 0.152f * (i + 1) + 0.1f };
		float2 textPadding = { 0.5f, 0.0f };
		float4 textColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		float2 textScale = { 0.3f, 0.3f };
		float4 textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

		Entity* entity = shopScene->AddEntity("upgrade" + std::to_string(i));
		gui = entity->AddComponent<component::GUI2DComponent>();
		gui->GetTextManager()->SetFont(m_pArial);
		gui->GetTextManager()->AddText("upgrade" + std::to_string(i));
		gui->GetTextManager()->SetColor(textColor, "upgrade" + std::to_string(i));
		gui->GetTextManager()->SetPadding(textPadding, "upgrade" + std::to_string(i));
		gui->GetTextManager()->SetPos(textPos, "upgrade" + std::to_string(i));
		gui->GetTextManager()->SetScale(textScale, "upgrade" + std::to_string(i));
		gui->GetTextManager()->SetText(textToRender, "upgrade" + std::to_string(i));
		gui->GetTextManager()->SetBlend(textBlend, "upgrade" + std::to_string(i));

		float2 quadPos = { 0.1f, 0.15f * (i + 1) + 0.099f };
		float2 quadScale = { 0.85f, 0.1f };
		float4 blended = { 1.0, 1.0, 1.0, 0.75 };
		float4 notBlended = { 1.0, 1.0, 1.0, 1.0 };

		Texture* shopBackground = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Upgrades/parchment_hor.png");
		gui->GetQuadManager()->CreateQuad(
			"upgrade" + std::to_string(i),
			quadPos, quadScale,
			false, false,
			2,
			notBlended,
			shopBackground);

		// add the entity to the sceneManager so it can be spawned in in run time
		entity->SetEntityState(true);	// true == dynamic, which means it will be removed when a new scene is set
		sm.AddEntity(entity, shopScene);
		/* ---------------------------------------------------------- */

		/* ------------------------- head --------------------------- */
		entity = shopScene->AddEntity("upgradebutton" + std::to_string(i));
		gui = entity->AddComponent<component::GUI2DComponent>();
		quadPos = { 0.01f, 0.15f * (i + 1) + 0.099f };
		quadScale = { 0.09f, 0.09f };
		Texture* shopImage = GetUpgradeImage(&GetInventoryNames().at(i));
		gui->GetQuadManager()->CreateQuad(
			"upgradebutton" + std::to_string(i),
			quadPos, quadScale,
			true, true,
			3,
			notBlended,
			shopImage
		);

		// add the entity to the sceneManager so it can be spawned in in run time
		entity->SetEntityState(true);	// true == dynamic, which means it will be removed when a new scene is set
		sm.AddEntity(entity, shopScene);
		/* ---------------------------------------------------------- */
	}

	// Reroll-button
	gui = nullptr;
	entity = shopScene->AddEntity("reroll");
	gui = entity->AddComponent<component::GUI2DComponent>();

	/*----------------Text-----------------*/
	std::string textToRender = "Reroll the inventory in the shop";
	textToRender += "\nPrice: ";
	textToRender += std::to_string(m_RerollCost);
	textToRender += " Coins";
	float2 textPos = { 0.76f, 0.66f };
	float2 textPadding = { 0.5f, 0.0f };
	float4 textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float2 textScale = { 0.3f, 0.3f };
	float4 textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

	gui->GetTextManager()->SetFont(m_pArial);
	gui->GetTextManager()->AddText("reroll");
	gui->GetTextManager()->SetColor(textColor, "reroll");
	gui->GetTextManager()->SetPadding(textPadding, "reroll");
	gui->GetTextManager()->SetPos(textPos, "reroll");
	gui->GetTextManager()->SetScale(textScale, "reroll");
	gui->GetTextManager()->SetText(textToRender, "reroll");
	gui->GetTextManager()->SetBlend(textBlend, "reroll");

	quadPos = { 0.75f, 0.66f };
	quadScale = { 0.2f, 0.3f };
	blended = { 1.0, 1.0, 1.0, 0.75 };
	notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		"reroll",
		quadPos, quadScale,
		false, false,
		2,
		blended,
		nullptr, { 0.0f, 0.0f, 0.0f });

	entity->SetEntityState(true);	// true == dynamic, which means it will be removed when a new scene is set
	sm.AddEntity(entity, shopScene);
	/*---------------------------------------*/

	/*---------------Texture-----------------*/
	entity = shopScene->AddEntity("reroll-button");
	gui = entity->AddComponent<component::GUI2DComponent>();
	quadPos = { 0.76f, 0.73f };
	quadScale = { 0.18, 0.18f };
	Texture* rerollImage = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Reroll.png");
	gui->GetQuadManager()->CreateQuad(
		"reroll-button",
		quadPos, quadScale,
		true, true,
		3,
		notBlended,
		rerollImage
	);
	entity->SetEntityState(true);	// true == dynamic, which means it will be removed when a new scene is set
	sm.AddEntity(entity, shopScene);
	/*---------------------------------------*/

	m_DisplayingShopGUI = true;
}

void Shop::Clear2DGUI()
{
	// If shop isn't active, we dont have anything to remove
	if (m_DisplayingShopGUI == false)
	{
		Log::PrintSeverity(Log::Severity::WARNING, "Trying to clear Shop2D-GUI when it doesn't exist!\n");
		return;
	}

	SceneManager& sm = SceneManager::GetInstance();
	Scene* shopScene = sm.GetScene("ShopScene");

	Entity* ent1 = nullptr;
	Entity* ent2 = nullptr;

	//Remove shop button and text
	for (int i = 0; i < GetInventorySize(); i++)
	{
		if (shopScene->EntityExists("upgrade" + std::to_string(i)))
		{
			ent1 = shopScene->GetEntity("upgrade" + std::to_string(i));
			ent2 = shopScene->GetEntity("upgradebutton" + std::to_string(i));

			sm.RemoveEntity(ent1, shopScene);
			sm.RemoveEntity(ent2, shopScene);
		}
	}

	ent1 = shopScene->GetEntity("background");
	sm.RemoveEntity(ent1, shopScene);

	//Removal of text and texture for reroll
	ent1 = shopScene->GetEntity("reroll");
	ent2 = shopScene->GetEntity("reroll-button");
	sm.RemoveEntity(ent1, shopScene);
	sm.RemoveEntity(ent2, shopScene);

	m_DisplayingShopGUI = false;
}

void Shop::ApplyUppgrade(std::string name)
{
	if (checkExisting(name))
	{
		// Increasing UpgradeManagers m_AppliedUpgradeLevel level as well as upgradeComponents m_AppliedUpgrades 
		// because we want to increase level of RANGE type upgrades as well and this needs to be done in UpgradeManager.
		if (m_AllAvailableUpgrades[name]->GetType() & F_UpgradeType::PLAYER)
		{
			m_pPlayer->GetComponent<component::UpgradeComponent>()->GetUpgradeByName(name)->IncreaseLevel();
		}
		m_pUpgradeManager->GetAllAvailableUpgrades().find(name)->second->ApplyStat();
		m_pUpgradeManager->IncreaseLevel(name);
	}
	else
	{
		m_pUpgradeManager->ApplyUpgrade(name);
		m_pUpgradeManager->IncreaseLevel(name);

		if (m_AllAvailableUpgrades[name]->GetType() & F_UpgradeType::PLAYER)
		{
			m_pPlayer->GetComponent<component::UpgradeComponent>()->GetUpgradeByName(name)->IncreaseLevel();
		}
	}
}

void Shop::SetInventorySize(int size)
{
	m_InvSize = size;
}

void Shop::SetPlayerBalance(int newBalance)
{
	m_pPlayer->GetComponent<component::CurrencyComponent>()->SetBalance(newBalance);
}

void Shop::ChangePlayerBalance(int change)
{
	m_pPlayer->GetComponent<component::CurrencyComponent>()->ChangeBalance(change);
}

int Shop::GetInventorySize()
{
	return m_InvSize;
}

std::vector<std::string> Shop::GetInventoryNames()
{
	return m_InventoryNames;
}

std::string Shop::GetUpgradeDescription(std::string name)
{
	return m_AllAvailableUpgrades.find(name)->second->GetDescription(m_AllAvailableUpgrades.find(name)->second->GetLevel());
}

int Shop::GetPrice(std::string name)
{
	return m_AllAvailableUpgrades.find(name)->second->GetPrice();
}

int Shop::GetPlayerBalance()
{
	return m_pPlayer->GetComponent<component::CurrencyComponent>()->GetBalace();
}

Texture* Shop::GetUpgradeImage(std::string* name)
{
	return AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/Upgrades/" + to_wstring(m_AllAvailableUpgrades[*name]->GetImage()));
}

bool Shop::IsShop2DGUIDisplaying()
{
	return m_DisplayingShopGUI;
}

void Shop::Reset()
{
	for (auto item : m_AllAvailableUpgrades)
	{
		item.second->SetLevel(0);
	}
	m_RerollCost = 50;
}

void Shop::OnShopGUIStateChange(shopGUIStateChange* event)
{
	// If the shopGUI is open, we close it
	if (IsShop2DGUIDisplaying() == true)
	{
		Clear2DGUI();

		// Remove Cursor
		ShowCursor(false);
	}

	// If the shopGUI is closed, we open it..
	else if(IsShop2DGUIDisplaying() == false)
	{
		// Only if the shopEntity is picked
		Entity* pickedEntity = Renderer::GetInstance().GetPickedEntity();
		if (pickedEntity != nullptr)
		{
			if (pickedEntity->GetName() == "shop")
			{
				Create2DGUI();

				// Reset movement, should happen here later. is currently happening in ShopSceneUpdateFunction in main
				//component::CollisionComponent* cc = Player::GetInstance().GetPlayer()->GetComponent<component::CollisionComponent>();
				//cc->SetVelVector(0.0f, 0.0f, 0.0f);

				// Show cursor
				ShowCursor(true);
			}
		}
	}
}

void Shop::shopButtonPressed(ButtonPressed* evnt)
{
	for (int i = 0; i < GetInventorySize(); i++)
	{
		if (evnt->name == "upgradebutton" + std::to_string(i) && m_InventoryIsBought.at(i) == false)
		{
			if (m_pPlayer->GetComponent<component::CurrencyComponent>()->GetBalace() >= GetPrice(m_InventoryNames.at(i)))
			{
				m_pPlayer->GetComponent<component::CurrencyComponent>()->ChangeBalance(-GetPrice(m_InventoryNames.at(i)));
				m_InventoryIsBought.at(i) = true;
				ApplyUppgrade(m_InventoryNames.at(i));

				SceneManager::GetInstance().GetActiveScene()->GetEntity("upgrade" + std::to_string(i))->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText(s_UpgradeBoughtText, "upgrade" + std::to_string(i));
			}
		}
	}

	if (evnt->name == "reroll-button")
	{
		//Clears the 2D-GUI, Rerolls the inventory of the shop and Creates the 2D-GUI with the new inventory.
		if (m_pPlayer->GetComponent<component::CurrencyComponent>()->GetBalace() >= m_RerollCost)
		{
			m_pPlayer->GetComponent<component::CurrencyComponent>()->ChangeBalance(-m_RerollCost);
			rerollPriceIncrease();
			rerollShop();
		}
	}
}

void Shop::sceneChange(SceneChange* evnt)
{
	if (evnt->m_NewSceneName == "ShopScene")
	{
		randomizeInventory();
	}
}

void Shop::rerollShop()
{
	randomizeInventory();
	Create2DGUI();
}

void Shop::rerollPriceIncrease()
{
	m_RerollCost += m_RerollIncrease;
}

void Shop::randomizeInventory()
{
	// start with clearing the inventory so we don't get the same upgrades as before.
	clearInventory();
	int upgradeNum;
	std::string name;
	bool inStock = false;

	// How many upgrades have reached max level
	int maxLevelUpgrades = 0;
	for (auto u : m_AllAvailableUpgrades)
	{
		if (u.second->GetLevel() == u.second->GetMaxLevel())
		{
			maxLevelUpgrades++;
		}
	}

	// If an upgrade is at max level, it will be unavailable for purchase.
	// So if we have less upgrades available for purchase than inventory size, we must lower inventory size.
	if (m_AllAvailableUpgrades.size() - maxLevelUpgrades < m_InvSize)
	{
		m_InvSize = m_AllAvailableUpgrades.size() - maxLevelUpgrades;
	}

	// Fill our inventory
	for (int i = 0; i < m_InvSize; i++)
	{
		// Set the seed for randomization of inventory
		m_Rand.SetSeed(time(NULL));
		// While loop is neccessary to avoid duplicate upgrades in inventory.
		do
		{
			// need to reset this bool every loop
			inStock = false;
			// Get a random number that will be used to get an upgrade to the inventory
			upgradeNum = m_Rand.Rand(0, m_AllAvailableUpgrades.size());
			// Take this number to get a name from m_UpgradeNames, which contains all avalible upgrades
			name = m_UpgradeNames.at(upgradeNum);
			for (auto names : m_InventoryNames)
			{
				if (name == names)
				{
					// If this name already exists in our inventory, 
					// then it is "inStock" so we have to try again.
					inStock = true;
					break;
				}
			}
			if (m_AllAvailableUpgrades[name]->GetLevel() == m_AllAvailableUpgrades[name]->GetMaxLevel())
			{
				// If an upgrade is at max level, make it unavailable for purchase
				// So set inStock to true so that it won't be added to the inventory.
				inStock = true;
			}

		} while (inStock);
		// When we get an upgrade that was not already in our inventory or max level,
		// we add it to the inventory.
		m_InventoryNames.push_back(name);
		m_InventoryIsBought.push_back(false);
	}
}

void Shop::clearInventory()
{
	// Remove quads
	Clear2DGUI();

	m_InventoryNames.clear();
	m_InventoryIsBought.clear();
}

bool Shop::checkExisting(std::string name)
{
	return m_pUpgradeManager->IsUpgradeApplied(name);
}
