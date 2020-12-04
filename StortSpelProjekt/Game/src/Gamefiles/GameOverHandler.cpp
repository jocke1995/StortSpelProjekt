#include "GameOverHandler.h"

#include "ECS/SceneManager.h"
#include "Misc/AssetLoader.h"
#include "ECS/Entity.h"
#include "Renderer/Texture/Texture2D.h"
#include "Misc/Option.h"
#include "Events/EventBus.h"
#include "Misc/GUI2DElements/Font.h"
#include "EnemyFactory.h"
#include "GameGUI.h"

void onGameOverSceneInit(Scene*);
void onMainMenu(const std::string& name);
// Declared in "MainMenuHandler.cpp".
void onExit(const std::string& name);
GameOverHandler& GameOverHandler::GetInstance()
{
    static GameOverHandler instance;
    return instance;
}

GameOverHandler::~GameOverHandler()
{
}

Scene* GameOverHandler::CreateScene(SceneManager* sm)
{
    AssetLoader* al = AssetLoader::Get();

    TextureCubeMap* blackCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/black.dds");
    Texture* mainMenuTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/MainMenu.png");
    Texture* exitTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Exit.png");
	Font* arial = al->LoadFontFromFile(L"Arial.fnt");

    component::SkyboxComponent* sbc = nullptr;
    component::GUI2DComponent* guiComp = nullptr;

    // Create Scene
    Scene* scene = sm->CreateScene("gameOverScene");

    // Player (Need a camera)
    Entity* entity = scene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);

    // Skybox
    entity = scene->AddEntity("skybox");
    sbc = entity->AddComponent<component::SkyboxComponent>();
    sbc->SetTexture(blackCubeMap);

    // Game over Text
    entity = scene->AddEntity("gameOverText");
    guiComp = entity->AddComponent<component::GUI2DComponent>();
	guiComp->GetTextManager()->SetFont(arial);
    guiComp->GetTextManager()->AddText("GameOverText");
    guiComp->GetTextManager()->SetScale({ 2, 2 }, "GameOverText");
    guiComp->GetTextManager()->SetPos({ 0.29, 0.10 }, "GameOverText");
    guiComp->GetTextManager()->SetText("Game Over", "GameOverText");

    // "You Died"
    entity = scene->AddEntity("youDiedText");
    guiComp = entity->AddComponent<component::GUI2DComponent>();
    guiComp->GetTextManager()->SetFont(arial);
    guiComp->GetTextManager()->AddText("youDiedText");
    guiComp->GetTextManager()->SetScale({ 0.6, 0.6 }, "youDiedText");
    guiComp->GetTextManager()->SetPos({ 0.43, 0.25 }, "youDiedText");
    guiComp->GetTextManager()->SetText("(You Died...)", "youDiedText");

    // Rounds Played
    entity = scene->AddEntity("levelPlayed");
    guiComp = entity->AddComponent<component::GUI2DComponent>();
    guiComp->GetTextManager()->SetFont(arial);
    guiComp->GetTextManager()->AddText("levelPlayed");
    guiComp->GetTextManager()->SetScale({ 0.6, 0.6 }, "levelPlayed");
    guiComp->GetTextManager()->SetPos({ 0.35, 0.35 }, "levelPlayed");
    guiComp->GetTextManager()->SetText("Level Reached: ", "levelPlayed");

    // KilledEnemies
    entity = scene->AddEntity("killedEnemies");
    guiComp = entity->AddComponent<component::GUI2DComponent>();
	guiComp->GetTextManager()->SetFont(arial);
    guiComp->GetTextManager()->AddText("killedEnemies");
    guiComp->GetTextManager()->SetScale({ 0.6, 0.6 }, "killedEnemies");
    guiComp->GetTextManager()->SetPos({ 0.35, 0.45 }, "killedEnemies");
    guiComp->GetTextManager()->SetText("Enemies Killed: ", "killedEnemies");

    // KilledEnemies
    entity = scene->AddEntity("timePlayed");
    guiComp = entity->AddComponent<component::GUI2DComponent>();
    guiComp->GetTextManager()->SetFont(arial);
    guiComp->GetTextManager()->AddText("timePlayed");
    guiComp->GetTextManager()->SetScale({ 0.6, 0.6 }, "timePlayed");
    guiComp->GetTextManager()->SetPos({ 0.35, 0.55 }, "timePlayed");
    guiComp->GetTextManager()->SetText("Time Played: ", "timePlayed");


#pragma region ReturnQuad
    entity = scene->AddEntity("MainMenuQuad");
    guiComp = entity->AddComponent<component::GUI2DComponent>();
    guiComp->GetQuadManager()->CreateQuad("MainMenuOption", { 0.1f, 0.8f }, { mainMenuTex->GetWidth() / 1920.0f, mainMenuTex->GetHeight() / 1080.0f }, true, true, 0, { 1.0,1.0,1.0,1.0 }, mainMenuTex);
    guiComp->GetQuadManager()->SetOnClicked(&onMainMenu);
#pragma endregion

#pragma region ExitQuad
    entity = scene->AddEntity("ExitQuad");
    guiComp = entity->AddComponent<component::GUI2DComponent>();
    guiComp->GetQuadManager()->CreateQuad("ExitOption", { 0.7f, 0.8f }, { exitTex->GetWidth() / 1920.0f, exitTex->GetHeight() / 1080.0f }, true, true, 0, { 1.0,1.0,1.0,1.0 }, exitTex);
    guiComp->GetQuadManager()->SetOnClicked(&onExit);
#pragma endregion

    scene->SetOnInit(&onGameOverSceneInit);

    m_pSceneManager = sm;
    m_pScene = scene;
    return scene;
}

Scene* GameOverHandler::GetScene()
{
    return m_pScene;
}

GameOverHandler::GameOverHandler()
{
};

void onGameOverSceneInit(Scene* scene)
{
    ShowCursor(true);
    scene->GetEntity("levelPlayed")->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText("Level Reached: " + std::to_string(EnemyFactory::GetInstance().GetLevel()), "levelPlayed");
    scene->GetEntity("killedEnemies")->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText("Enemies Killed: " + std::to_string(EnemyFactory::GetInstance().GetTotalKilled()), "killedEnemies");
    int seconds = GameGUI::GetInstance().GetTimePlayed();
    int minute = seconds / 60;
    seconds = seconds % 60;
    scene->GetEntity("timePlayed")->GetComponent<component::GUI2DComponent>()->GetTextManager()->SetText("Time Played: " + std::to_string(minute) + ":" + std::to_string(seconds), "timePlayed");
}

void onMainMenu(const std::string& name)
{
    EventBus::GetInstance().Publish(&ResetGame());
    EventBus::GetInstance().Publish(&SceneChange("MainMenuScene"));
}