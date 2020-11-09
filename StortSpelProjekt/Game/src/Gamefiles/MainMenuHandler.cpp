#include "MainMenuHandler.h"
#include "ECS/SceneManager.h"
#include "Misc/AssetLoader.h"
#include "ECS/Entity.h"
#include "Renderer/Texture/Texture2D.h"
#include "Misc/Option.h"
#include "Events/EventBus.h"
#include "Renderer/Renderer.h"
#include "Misc/Window.h"

void onStart(const std::string& name);
void onExit(const std::string& name);
void onOptions(const std::string& name);
void onOptionBack(const std::string& name);
void on1920x1080(const std::string& name);
void on1280x720(const std::string& name);

MainMenuHandler::MainMenuHandler()
{
}

void MainMenuHandler::createOptionScene()
{
    AssetLoader* al = AssetLoader::Get();

    m_pOptionScene = m_pSceneManager->CreateScene("OptionScene");

    component::GUI2DComponent* guic = nullptr;
    component::Audio2DVoiceComponent* vc = nullptr;

    Texture* exitTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Back.png");
    Texture* res1080p = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/1920x1080.png");
    Texture* res720p = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/1280x720.png");

    // Player (Need a camera)
    Entity* entity = m_pOptionScene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    vc = entity->AddComponent<component::Audio2DVoiceComponent>();

    // Skybox
    entity = m_pOptionScene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    TextureCubeMap* blackCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/black.dds");
    sbc->SetTexture(blackCubeMap);

    entity = m_pOptionScene->AddEntity("1080p");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("1080p", { 0.3f, 0.2f }, { (float)res1080p->GetWidth() / std::stoi(Option::GetInstance().GetVariable("i_resolutionWidth")), (float)res1080p->GetHeight() / std::stoi(Option::GetInstance().GetVariable("i_resolutionHeight")) }, true, true, 0, { 1.0,1.0,1.0,1.0 }, res1080p);
    guic->GetQuadManager()->SetOnClicked(&on1920x1080);
    entity = m_pOptionScene->AddEntity("720p");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("720p", { 0.3f, 0.3f }, { (float)res720p->GetWidth() / std::stoi(Option::GetInstance().GetVariable("i_resolutionWidth")), (float)res720p->GetHeight() / std::stoi(Option::GetInstance().GetVariable("i_resolutionHeight")) }, true, true, 0, { 1.0,1.0,1.0,1.0 }, res720p);
    guic->GetQuadManager()->SetOnClicked(&on1280x720);

    entity = m_pOptionScene->AddEntity("Back");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("Back", { 0.05f, 0.8f }, { (float)exitTex->GetWidth() / std::stoi(Option::GetInstance().GetVariable("i_resolutionWidth")), (float)exitTex->GetHeight() / std::stoi(Option::GetInstance().GetVariable("i_resolutionHeight")) }, true, true, 0, { 1.0,1.0,1.0,1.0 }, exitTex);
    guic->GetQuadManager()->SetOnClicked(&onOptionBack);
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
    component::Audio2DVoiceComponent* vc = nullptr;

    Texture* startTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Start.png");
    Texture* optionsTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Options.png");
    Texture* exitTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Exit.png");

    AudioBuffer* menuSound = al->LoadAudio(L"../Vendor/Resources/Audio/Menu.wav", L"MenuMusic");

    // Player (Need a camera)
    Entity* entity = scene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    // Add a voice to the player to play some music.
    vc = entity->AddComponent<component::Audio2DVoiceComponent>();
    vc->AddVoice(L"MenuMusic");
    vc->Play(L"MenuMusic");

    // Skybox
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    TextureCubeMap* blackCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/black.dds");
    sbc->SetTexture(blackCubeMap);

    // Since the target platform uses 1920 : 1080 ratio we use this same ratio for our texture sizes.

    entity = scene->AddEntity("StartOption");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("StartOption", { 0.1f, 0.1f }, { startTex->GetWidth() / 1920.0f, startTex->GetHeight() / 1080.0f }, true, true, 0, { 1.0,1.0,1.0,1.0 }, startTex);
    guic->GetQuadManager()->SetOnClicked(&onStart);

    entity = scene->AddEntity("OptionsOption");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("OptionsOption", { 0.1f, 0.2f }, { optionsTex->GetWidth() / 1920.0f, optionsTex->GetHeight() / 1080.0f }, true, true, 0, { 1.0,1.0,1.0,1.0 }, optionsTex);

    entity = scene->AddEntity("ExitOption");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("ExitOption", { 0.1f, 0.3f }, { exitTex->GetWidth() / 1920.0f, exitTex->GetHeight() / 1080.0f }, true, true, 0, { 1.0,1.0,1.0,1.0 }, exitTex);
    guic->GetQuadManager()->SetOnClicked(&onExit);
    m_pSceneManager = sm;
    m_pScene = scene;

    createOptionScene();

    return scene;
}

Scene* MainMenuHandler::GetScene()
{
    return m_pScene;
}

void onStart(const std::string& name)
{
    EventBus::GetInstance().Publish(&SceneChange("GameScene"));
    EventBus::GetInstance().Publish(&CursorShow(false));
    EventBus::GetInstance().Publish(&RoundStart());
}

void onExit(const std::string& name)
{
    EventBus::GetInstance().Publish(&ShutDown());
}

void onOptions(const std::string& name)
{
    EventBus::GetInstance().Publish(&SceneChange("OptionScene"));
}

void onOptionBack(const std::string& name)
{
    EventBus::GetInstance().Publish(&SceneChange("MainMenuScene"));
}

void on1920x1080(const std::string& name)
{
    Option::GetInstance().SetVariable("i_resolutionWidth", "1920");
    Option::GetInstance().SetVariable("i_resolutionHeight", "1080");

    //If we are not in fullscreen we should change the window to match the resolution
    if (Renderer::GetInstance().GetWindow()->IsFullScreen() == false)
    {
        Option::GetInstance().SetVariable("i_windowWidth", "1920");
        Option::GetInstance().SetVariable("i_windowHeight", "1080");
    }

    Option::GetInstance().WriteFile();

    EventBus::GetInstance().Publish(&WindowChange());
}

void on1280x720(const std::string& name)
{
    Option::GetInstance().SetVariable("i_resolutionWidth", "1280");
    Option::GetInstance().SetVariable("i_resolutionHeight", "720");

    //If we are not in fullscreen we should change the window to match the resolution
    if (Renderer::GetInstance().GetWindow()->IsFullScreen() == false)
    {
        Option::GetInstance().SetVariable("i_windowWidth", "1280");
        Option::GetInstance().SetVariable("i_windowHeight", "720");
    }

    Option::GetInstance().WriteFile();

    EventBus::GetInstance().Publish(&WindowChange());
}
