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
void MainMenuUpdateScene(SceneManager* sm, double dt);

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

    Model* enemyModel = al->LoadModel(L"../Vendor/Resources/Models/Zombie/zombie.obj");
    entity = scene->AddEntity("Zombie");
    component::ModelComponent* mc = entity->AddComponent<component::ModelComponent>();
    component::TransformComponent* tc = entity->AddComponent<component::TransformComponent>();
    mc->SetModel(enemyModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    Transform* t = tc->GetTransform();
    t->SetPosition(0.0, 0.0, 10.0);
    t->SetScale(0.03);
    t->SetRotationX(0.0);
    t->SetRotationY(PI);
    t->SetRotationZ(0.0);
    tc->SetTransformOriginalState();

    double3 playerDim = mc->GetModelDim();

    double rad = playerDim.z / 2.0;
    double cylHeight = playerDim.y - (rad * 2.0);
    component::CollisionComponent* cc = entity->AddComponent<component::CapsuleCollisionComponent>(200.0, rad, cylHeight, 0.0, 0.0, false);
    cc->SetGravity(0.0);
    /* ----------------- Light ------------------- */

    entity = scene->AddEntity("SpotLight");

    component::SpotLightComponent* slc = entity->AddComponent<component::SpotLightComponent>();
    slc->SetColor({ 4.0f, 4.0f, 4.0f });
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
    slc->SetDirection({ 0.0, -1.0, 0.0f });
    slc->SetPosition({ 0.0, 10.0, 10.0 });

    m_pSceneManager = sm;
    m_pScene = scene;

    scene->SetUpdateScene(&MainMenuUpdateScene);

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
}

void MainMenuUpdateScene(SceneManager* sm, double dt)
{
    static float rotValue = 0.0f;
    Transform* trans = sm->GetScene("MainMenuScene")->GetEntity("Zombie")->GetComponent<component::TransformComponent>()->GetTransform();
    trans->SetRotationY(rotValue);
    trans->SetPosition({ 0.0, std::sin(rotValue), 10.0 });

    rotValue += dt;
}