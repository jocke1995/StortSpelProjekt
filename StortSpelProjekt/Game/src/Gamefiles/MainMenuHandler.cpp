#include "MainMenuHandler.h"
#include "ECS/SceneManager.h"
#include "Misc/AssetLoader.h"
#include "ECS/Entity.h"
#include "Renderer/Texture/Texture2D.h"
#include "Misc/Option.h"
#include "Events/EventBus.h"
void onStart(const std::string& name);
void onExit(const std::string& name);
void onOptions(const std::string& name);

MainMenuHandler::MainMenuHandler()
{
}

MainMenuHandler& MainMenuHandler::GetInstance()
{
    static MainMenuHandler instance;
    return instance;
}

MainMenuHandler::~MainMenuHandler()
{
}

Scene* MainMenuHandler::CreateScene(SceneManager* sm)
{
    AssetLoader* al = AssetLoader::Get();

    Scene* scene = sm->CreateScene("MainMenuScene");

    component::GUI2DComponent* guic = nullptr;

    Texture* startTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Start.png");
    Texture* optionsTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Options.png");
    Texture* exitTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Exit.png");

    // Player (Need a camera)
    Entity* entity = scene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);

    // Skybox
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    TextureCubeMap* blackCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/black.dds");
    sbc->SetTexture(blackCubeMap);

    entity = scene->AddEntity("StartOption");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("StartOption", { 0.1f, 0.1f }, { (float)startTex->GetWidth() / std::stoi(Option::GetInstance().GetVariable("i_resolutionWidth")), (float)startTex->GetHeight() / std::stoi(Option::GetInstance().GetVariable("i_resolutionHeight")) }, true, true, 0, { 1.0,1.0,1.0,1.0 }, startTex);
    guic->GetQuadManager()->SetOnClicked(&onStart);

    entity = scene->AddEntity("OptionsOption");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("OptionsOption", { 0.1f, 0.2f }, { (float)optionsTex->GetWidth() / std::stoi(Option::GetInstance().GetVariable("i_resolutionWidth")), (float)optionsTex->GetHeight() / std::stoi(Option::GetInstance().GetVariable("i_resolutionHeight")) }, true, true, 0, { 1.0,1.0,1.0,1.0 }, optionsTex);

    entity = scene->AddEntity("ExitOption");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("ExitOption", { 0.1f, 0.3f }, { (float)exitTex->GetWidth() / std::stoi(Option::GetInstance().GetVariable("i_resolutionWidth")), (float)exitTex->GetHeight() / std::stoi(Option::GetInstance().GetVariable("i_resolutionHeight")) }, true, true, 0, { 1.0,1.0,1.0,1.0 }, exitTex);
    guic->GetQuadManager()->SetOnClicked(&onExit);
    m_pSceneManager = sm;
    m_pScene = scene;
    return scene;
}

Scene* MainMenuHandler::GetScene()
{
    return m_pScene;
}

void onStart(const std::string& name)
{
    EventBus::GetInstance().Publish(&CursorShow(false));
    EventBus::GetInstance().Publish(&SceneChange("GameScene"));
}

void onExit(const std::string& name)
{
    EventBus::GetInstance().Publish(&ShutDown());
}

void onOptions(const std::string& name)
{
}
