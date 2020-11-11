#include "MainMenuHandler.h"
#include "ECS/SceneManager.h"
#include "Misc/AssetLoader.h"
#include "ECS/Entity.h"
#include "Renderer/Texture/Texture2D.h"
#include "Misc/Option.h"
#include "Events/EventBus.h"
void onMainMenuSceneInit(Scene* scene);
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
    guic->GetQuadManager()->CreateQuad("OptionsOption", { 0.1f, 0.25f }, { optionsTex->GetWidth() / 1920.0f, optionsTex->GetHeight() / 1080.0f }, true, true, 0, { 1.0,1.0,1.0,1.0 }, optionsTex);

    entity = scene->AddEntity("ExitOption");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("ExitOption", { 0.1f, 0.4f }, { exitTex->GetWidth() / 1920.0f, exitTex->GetHeight() / 1080.0f }, true, true, 0, { 1.0,1.0,1.0,1.0 }, exitTex);
    guic->GetQuadManager()->SetOnClicked(&onExit);
    scene->SetOnInit(&onMainMenuSceneInit);
    m_pSceneManager = sm;
    m_pScene = scene;
    return scene;
}

Scene* MainMenuHandler::GetScene()
{
    return m_pScene;
}

void onMainMenuSceneInit(Scene* scene)
{
    scene->GetEntity("player")->GetComponent<component::Audio2DVoiceComponent>()->Play(L"MenuMusic");
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
}
