#include "GameOverHandler.h"

#include "ECS/SceneManager.h"
#include "Misc/AssetLoader.h"
#include "ECS/Entity.h"
#include "Renderer/Texture/Texture2D.h"
#include "Misc/Option.h"
#include "Events/EventBus.h"
#include "Misc/GUI2DElements/Font.h"

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
    guiComp->GetTextManager()->SetPos({ 0.29, 0.41 }, "GameOverText");
    guiComp->GetTextManager()->SetText("Game Over", "GameOverText");

    // text2
    entity = scene->AddEntity("youDiedText");
    guiComp = entity->AddComponent<component::GUI2DComponent>();
	guiComp->GetTextManager()->SetFont(arial);
    guiComp->GetTextManager()->AddText("youDiedText");
    guiComp->GetTextManager()->SetScale({ 0.6, 0.6 }, "youDiedText");
    guiComp->GetTextManager()->SetPos({ 0.43, 0.56 }, "youDiedText");
    guiComp->GetTextManager()->SetText("(You Died...)", "youDiedText");

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
}

void onMainMenu(const std::string& name)
{
    EventBus::GetInstance().Publish(&ResetGame());
    EventBus::GetInstance().Publish(&SceneChange("MainMenuScene"));
}