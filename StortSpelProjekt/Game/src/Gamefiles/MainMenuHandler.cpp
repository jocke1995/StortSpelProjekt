#include "MainMenuHandler.h"
#include "ECS/SceneManager.h"
#include "Misc/AssetLoader.h"
#include "ECS/Entity.h"
#include "Renderer/Texture/Texture2D.h"
#include "Misc/Option.h"
#include "Events/EventBus.h"
#include "Events/Events.h"
#include "Renderer/Renderer.h"
#include "Misc/Window.h"
#include "Misc/GUI2DElements/Font.h"
#include <iomanip>
#include <sstream>

void onMainMenuSceneInit(Scene* scene);
void onStart(const std::string& name);
void onExit(const std::string& name);
void onOptions(const std::string& name);
void onOptionBack(const std::string& name);
void on2560x1440(const std::string& name);
void on1920x1080(const std::string& name);
void on1280x720(const std::string& name);
void onFullscreen(const std::string& name);
void onWindowed(const std::string& name);
void onLowShadowQuality(const std::string& name);
void onMedShadowQuality(const std::string& name);
void onHighShadowQuality(const std::string& name);
void onVolumePlus(const std::string& name);
void onVolumeMinus(const std::string& name);
void MainMenuUpdateScene(SceneManager* sm, double dt);

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
    Texture* resolution = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Resolution.png");
    Texture* restart = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/RestartWarning.png");
    Texture* res1440p = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/2560x1440.png");
    Texture* res1080p = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/1920x1080.png");
    Texture* res720p = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/1280x720.png");
    Texture* fullscreen = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Fullscreen.png");
    Texture* windowed = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Windowed.png");
    Texture* shadowQuality = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/ShadowQuality.png");
    Texture* lowShadowQuality = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Low.png");
    Texture* medShadowQuality = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Med.png");
    Texture* highShadowQuality = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/High.png");
    Texture* volume = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Volume.png");
    Texture* plus = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Plus.png");
    Texture* minus = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Minus.png");

	Font* arial = al->LoadFontFromFile(L"Arial.fnt");

    // Player (Need a camera)
    Entity* entity = m_pOptionScene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    vc = entity->AddComponent<component::Audio2DVoiceComponent>();

    // Skybox
    entity = m_pOptionScene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    TextureCubeMap* blackCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/black.dds");
    sbc->SetTexture(blackCubeMap);

    /*------------Resolution------------*/
    entity = m_pOptionScene->AddEntity("resolution");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("resolution",
        { 0.35f, 0.05f },
        { (float)resolution->GetWidth() / 1920.0f, (float)resolution->GetHeight() / 1080.0f },
        false,
        false,
        1,
        { 1.0,1.0,1.0,1.0 },
        resolution);
    entity = m_pOptionScene->AddEntity("1444p");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("1444p",
        { 0.35f, 0.15f },
        { (float)res1440p->GetWidth() / 1920.0f, (float)res1440p->GetHeight() / 1080.0f },
        true,
        true,
        1,
        { 1.0,1.0,1.0,1.0 },
        res1440p);
    guic->GetQuadManager()->SetOnClicked(&on2560x1440);
    entity = m_pOptionScene->AddEntity("1080p");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("1080p",
        { 0.35f, 0.25f },
        { (float)res1080p->GetWidth() / 1920.0f, (float)res1080p->GetHeight() / 1080.0f },
        true,
        true,
        1,
        { 1.0,1.0,1.0,1.0 },
        res1080p);
    guic->GetQuadManager()->SetOnClicked(&on1920x1080);
    entity = m_pOptionScene->AddEntity("720p");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("720p",
        { 0.35f, 0.35f },
        { (float)res720p->GetWidth() / 1920.0f, (float)res720p->GetHeight() / 1080.0f },
        true,
        true,
        1,
        { 1.0,1.0,1.0,1.0 },
        res720p);
    guic->GetQuadManager()->SetOnClicked(&on1280x720);

    entity = m_pOptionScene->AddEntity("activeResolution");
    guic = entity->AddComponent<component::GUI2DComponent>();
    switch (std::stoi(Option::GetInstance().GetVariable("i_resolutionHeight")))
    {
    case 720:
        guic->GetQuadManager()->CreateQuad("activeResolution",
            { 0.35f, 0.35f },
            { (float)res720p->GetWidth() / 1920.0f, (float)res720p->GetHeight() / 1080.0f },
            false,
            false,
            0,
            { 1.0,1.0,1.0,1.0 },
            nullptr,
            { 0.0f, 1.0f, 0.0f });
        break;
    case 1080:
        guic->GetQuadManager()->CreateQuad("activeResolution",
            { 0.35f, 0.25f },
            { (float)res1080p->GetWidth() / 1920.0f, (float)res1080p->GetHeight() / 1080.0f },
            false,
            false,
            0,
            { 1.0, 1.0, 1.0, 1.0 },
            nullptr,
            { 0.0f, 1.0f, 0.0f });
        break;
    case 1440:
        guic->GetQuadManager()->CreateQuad("1444p",
            { 0.35f, 0.15f },
            { (float)res1440p->GetWidth() / 1920.0f, (float)res1440p->GetHeight() / 1080.0f },
            false,
            false,
            0,
            { 1.0,1.0,1.0,1.0 },
            nullptr,
            { 0.0f, 1.0f, 0.0f });
        break;
    }

    /*-------------Window Mode-------------------*/
    entity = m_pOptionScene->AddEntity("Fullscreen");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("Fullscreen",
        { 0.60f, 0.15f },
        { (float)fullscreen->GetWidth() / 1920.0f, (float)fullscreen->GetHeight() / 1080.0f },
        true,
        true,
        1,
        { 1.0,1.0,1.0,1.0 },
        fullscreen);
    guic->GetQuadManager()->SetOnClicked(&onFullscreen);

    entity = m_pOptionScene->AddEntity("Windowed");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("Windowed",
        { 0.60f, 0.25f },
        { (float)windowed->GetWidth() / 1920.0f, (float)windowed->GetHeight() / 1080.0f },
        true,
        true,
        1,
        { 1.0,1.0,1.0,1.0 },
        windowed);
    guic->GetQuadManager()->SetOnClicked(&onWindowed);

    entity = m_pOptionScene->AddEntity("activeWindowmode");
    guic = entity->AddComponent<component::GUI2DComponent>();
    switch (std::stoi(Option::GetInstance().GetVariable("i_windowMode")))
    {
    case 1:
        guic->GetQuadManager()->CreateQuad("activeWindowmode",
            { 0.60f, 0.15f },
            { (float)fullscreen->GetWidth() / 1920.0f, (float)fullscreen->GetHeight() / 1080.0f },
            false,
            false,
            0,
            { 1.0,1.0,1.0,1.0 },
            nullptr,
            { 0.0f, 1.0f, 0.0f });
        break;
    case 0:
        guic->GetQuadManager()->CreateQuad("activeWindowmode",
            { 0.60f, 0.25f },
            { (float)windowed->GetWidth() / 1920.0f, (float)windowed->GetHeight() / 1080.0f },
            false,
            false,
            0,
            { 1.0, 1.0, 1.0, 1.0 },
            nullptr,
            { 0.0f, 1.0f, 0.0f });
        break;
    }

    /*-------------Shadow Quality--------------*/
    entity = m_pOptionScene->AddEntity("shadowQuality");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("shadowQuality",
        { 0.32f, 0.50f },
        { (float)shadowQuality->GetWidth() / 1920.0f, (float)shadowQuality->GetHeight() / 1080.0f },
        false,
        false,
        0,
        { 1.0,1.0,1.0,1.0 },
        shadowQuality);

    entity = m_pOptionScene->AddEntity("lowShadowQuality");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("lowShadowQuality",
        { 0.35f, 0.60f },
        { (float)lowShadowQuality->GetWidth() / 1920.0f, (float)lowShadowQuality->GetHeight() / 1080.0f },
        true,
        true,
        1,
        { 1.0,1.0,1.0,1.0 },
        lowShadowQuality);
    guic->GetQuadManager()->SetOnClicked(&onLowShadowQuality);
    entity = m_pOptionScene->AddEntity("medShadowQuality");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("medShadowQuality",
        { 0.35f, 0.70f },
        { (float)medShadowQuality->GetWidth() / 1920.0f, (float)medShadowQuality->GetHeight() / 1080.0f },
        true,
        true,
        1,
        { 1.0,1.0,1.0,1.0 },
        medShadowQuality);
    guic->GetQuadManager()->SetOnClicked(&onMedShadowQuality);
    entity = m_pOptionScene->AddEntity("highShadowQuality");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("highShadowQuality",
        { 0.35f, 0.80f },
        { (float)highShadowQuality->GetWidth() / 1920.0f, (float)highShadowQuality->GetHeight() / 1080.0f },
        true,
        true,
        1,
        { 1.0,1.0,1.0,1.0 },
        highShadowQuality);
    guic->GetQuadManager()->SetOnClicked(&onHighShadowQuality);

    entity = m_pOptionScene->AddEntity("activeShadowQuality");
    guic = entity->AddComponent<component::GUI2DComponent>();
    switch (std::stoi(Option::GetInstance().GetVariable("i_shadowResolution")))
    {
    case 0:
        guic->GetQuadManager()->CreateQuad("activeShadowQuality",
            { 0.35f, 0.60f },
            { (float)lowShadowQuality->GetWidth() / 1920.0f, (float)lowShadowQuality->GetHeight() / 1080.0f },
            false,
            false,
            0,
            { 1.0,1.0,1.0,1.0 },
            nullptr,
            { 0.0f, 1.0f, 0.0f });
        break;
    case 1:
        guic->GetQuadManager()->CreateQuad("activeShadowQuality",
            { 0.35f, 0.70f },
            { (float)medShadowQuality->GetWidth() / 1920.0f, (float)medShadowQuality->GetHeight() / 1080.0f },
            false,
            false,
            0,
            { 1.0, 1.0, 1.0, 1.0 },
            nullptr,
            { 0.0f, 1.0f, 0.0f });
        break;
    case 2:
        guic->GetQuadManager()->CreateQuad("activeShadowQuality",
            { 0.35f, 0.80f },
            { (float)highShadowQuality->GetWidth() / 1920.0f, (float)highShadowQuality->GetHeight() / 1080.0f },
            false,
            false,
            0,
            { 1.0, 1.0, 1.0, 1.0 },
            nullptr,
            { 0.0f, 1.0f, 0.0f });
        break;
    }
    /*-------------Volume--------------*/
    entity = m_pOptionScene->AddEntity("volumeText");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("volumeText",
        { 0.65f, 0.52f },
        { (float)volume->GetWidth() / 1920.0f, (float)volume->GetHeight() / 1080.0f },
        false,
        false,
        1,
        { 1.0,1.0,1.0,1.0 },
        volume);

    entity = m_pOptionScene->AddEntity("volumePlus");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("volumePlus",
        { 0.66f, 0.70f },
        { (float)plus->GetWidth() / 1920.0f, (float)plus->GetHeight() / 1080.0f },
        true,
        true,
        1,
        { 1.0,1.0,1.0,1.0 },
        plus);
    guic->GetQuadManager()->SetOnClicked(&onVolumePlus);

    entity = m_pOptionScene->AddEntity("volumeMinus");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("volumeMinus",
        { 0.75f, 0.70f },
        { (float)minus->GetWidth() / 1920.0f, (float)minus->GetHeight() / 1080.0f },
        true,
        true,
        1,
        { 1.0,1.0,1.0,1.0 },
        minus);
    guic->GetQuadManager()->SetOnClicked(&onVolumeMinus);

    std::string textToRender = Option::GetInstance().GetVariable("f_volume");
    float2 textPos = { 0.7f, 0.62f };
    float2 textPadding = { 0.5f, 0.0f };
    float4 textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float2 textScale = { 1.0f, 1.0f };
    float4 textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

    entity = m_pOptionScene->AddEntity("volume");
    guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetTextManager()->SetFont(arial);
    guic->GetTextManager()->AddText("volume");
    guic->GetTextManager()->SetColor(textColor, "volume");
    guic->GetTextManager()->SetPadding(textPadding, "volume");
    guic->GetTextManager()->SetPos(textPos, "volume");
    guic->GetTextManager()->SetScale(textScale, "volume");
    guic->GetTextManager()->SetText(textToRender, "volume");
    guic->GetTextManager()->SetBlend(textBlend, "volume");

    guic->GetQuadManager()->CreateQuad(
        "money",
        { 0.7f, 0.65f }, { 0.1f, 0.1f },
        false, false,
        1,
        { 1.0, 1.0, 1.0, 0.0 },
        nullptr
    );

    /*-------------Back--------------*/
    entity = m_pOptionScene->AddEntity("restartWarning");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("restartWarning",
        { 0.03f, 0.55f },
        { (float)restart->GetWidth() / 1920.0f, (float)restart->GetHeight() / 1080.0f },
        false,
        false,
        0,
        { 1.0,1.0,1.0,1.0 },
        restart);


    entity = m_pOptionScene->AddEntity("Back");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("Back",
        { 0.05f, 0.8f },
        { (float)exitTex->GetWidth() / 1920.0f, (float)exitTex->GetHeight() / 1080.0f },
        true,
        true,
        0,
        { 1.0,1.0,1.0,1.0 },
        exitTex);
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
    guic->GetQuadManager()->SetOnClicked(&onOptions);

    entity = scene->AddEntity("ExitOption");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("ExitOption", { 0.1f, 0.4f }, { exitTex->GetWidth() / 1920.0f, exitTex->GetHeight() / 1080.0f }, true, true, 0, { 1.0,1.0,1.0,1.0 }, exitTex);
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

    scene->SetOnInit(&onMainMenuSceneInit);
    m_pSceneManager = sm;
    m_pScene = scene;

    createOptionScene();


    scene->SetUpdateScene(&MainMenuUpdateScene);

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

Scene* MainMenuHandler::GetOptionScene()
{
    return m_pOptionScene;
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

void on2560x1440(const std::string& name)
{
    Option::GetInstance().SetVariable("i_resolutionWidth", "2560");
    Option::GetInstance().SetVariable("i_resolutionHeight", "1440");

    Option::GetInstance().SetVariable("i_windowWidth", "2560");
    Option::GetInstance().SetVariable("i_windowHeight", "1440");

    Option::GetInstance().WriteFile();

    Texture* high = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/2560x1440.png");
    MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeResolution")->GetComponent<component::GUI2DComponent>()->GetQuadManager()->UpdateQuad(
        { 0.35f, 0.15f },
        { (float)high->GetWidth() / 1920.0f, (float)high->GetHeight() / 1080.0f },
        false,
        false,
        { 1.0, 1.0, 1.0, 1.0 },
        { 0.0f, 1.0f, 0.0f });
}

void on1920x1080(const std::string& name)
{
    Option::GetInstance().SetVariable("i_resolutionWidth", "1920");
    Option::GetInstance().SetVariable("i_resolutionHeight", "1080");

    Option::GetInstance().SetVariable("i_windowWidth", "1920");
    Option::GetInstance().SetVariable("i_windowHeight", "1080");

    Option::GetInstance().WriteFile();

    Texture* med = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/1920x1080.png");
    MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeResolution")->GetComponent<component::GUI2DComponent>()->GetQuadManager()->UpdateQuad(
        { 0.35f, 0.25f },
        { (float)med->GetWidth() / 1920.0f, (float)med->GetHeight() / 1080.0f },
        false,
        false,
        { 1.0, 1.0, 1.0, 1.0 },
        { 0.0f, 1.0f, 0.0f });
}

void on1280x720(const std::string& name)
{
    Option::GetInstance().SetVariable("i_resolutionWidth", "1280");
    Option::GetInstance().SetVariable("i_resolutionHeight", "720");

    Option::GetInstance().SetVariable("i_windowWidth", "1280");
    Option::GetInstance().SetVariable("i_windowHeight", "720");

    Option::GetInstance().WriteFile();

    Texture* low = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/1280x720.png");
    MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeResolution")->GetComponent<component::GUI2DComponent>()->GetQuadManager()->UpdateQuad(
        { 0.35f, 0.35f },
        { (float)low->GetWidth() / 1920.0f, (float)low->GetHeight() / 1080.0f },
        false,
        false,
        { 1.0, 1.0, 1.0, 1.0 },
        { 0.0f, 1.0f, 0.0f });
}

void onFullscreen(const std::string& name)
{
    Option::GetInstance().SetVariable("i_windowMode", "1");

    Option::GetInstance().WriteFile();

    Texture* fullscreen = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Fullscreen.png");
    MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeWindowmode")->GetComponent<component::GUI2DComponent>()->GetQuadManager()->UpdateQuad(
        { 0.60f, 0.15f },
        { (float)fullscreen->GetWidth() / 1920.0f, (float)fullscreen->GetHeight() / 1080.0f },
        false,
        false,
        { 1.0, 1.0, 1.0, 1.0 },
        { 0.0f, 1.0f, 0.0f });
}

void onWindowed(const std::string& name)
{
    Option::GetInstance().SetVariable("i_windowMode", "0");

    Option::GetInstance().WriteFile();

    Texture* window = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Windowed.png");
    MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeWindowmode")->GetComponent<component::GUI2DComponent>()->GetQuadManager()->UpdateQuad(
        { 0.60f, 0.25f },
        { (float)window->GetWidth() / 1920.0f, (float)window->GetHeight() / 1080.0f },
        false,
        false,
        { 1.0, 1.0, 1.0, 1.0 },
        { 0.0f, 1.0f, 0.0f });
}

void onLowShadowQuality(const std::string& name)
{
    Option::GetInstance().SetVariable("i_shadowResolution", "0");

    Option::GetInstance().WriteFile();

    Texture* low = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Low.png");
    MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeShadowQuality")->GetComponent<component::GUI2DComponent>()->GetQuadManager()->UpdateQuad(
        { 0.35f, 0.60f },
        { (float)low->GetWidth() / 1920.0f, (float)low->GetHeight() / 1080.0f },
        false,
        false,
        { 1.0, 1.0, 1.0, 1.0 },
        { 0.0f, 1.0f, 0.0f });
}

void onMedShadowQuality(const std::string& name)
{
    Option::GetInstance().SetVariable("i_shadowResolution", "1");

    Option::GetInstance().WriteFile();

    Texture* med = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Med.png");
    MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeShadowQuality")->GetComponent<component::GUI2DComponent>()->GetQuadManager()->UpdateQuad(
        { 0.35f, 0.70f },
        { (float)med->GetWidth() / 1920.0f, (float)med->GetHeight() / 1080.0f },
        false,
        false,
        { 1.0, 1.0, 1.0, 1.0 },
        { 0.0f, 1.0f, 0.0f });
}

void onHighShadowQuality(const std::string& name)
{
    Option::GetInstance().SetVariable("i_shadowResolution", "2");

    Option::GetInstance().WriteFile();

    Texture* high = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/High.png");
    MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeShadowQuality")->GetComponent<component::GUI2DComponent>()->GetQuadManager()->UpdateQuad(
        { 0.35f, 0.80f },
        { (float)high->GetWidth() / 1920.0f, (float)high->GetHeight() / 1080.0f },
        false,
        false,
        { 1.0, 1.0, 1.0, 1.0 },
        { 0.0f, 1.0f, 0.0f });
}

void onVolumePlus(const std::string& name)
{
    if (std::stof(Option::GetInstance().GetVariable("f_volume")) < 10)
    {
        std::ostringstream str;
        str << std::setprecision(2) << std::stof(Option::GetInstance().GetVariable("f_volume")) + 0.1f;
        Option::GetInstance().SetVariable("f_volume", str.str());

        Option::GetInstance().WriteFile();
    }
}

void onVolumeMinus(const std::string& name)
{
    if (std::stof(Option::GetInstance().GetVariable("f_volume")) > 0)
    {
        std::ostringstream str;
        str << std::setprecision(2) << std::stof(Option::GetInstance().GetVariable("f_volume")) - 0.1f;
        Option::GetInstance().SetVariable("f_volume", str.str());

        Option::GetInstance().WriteFile();
    }
}

void MainMenuUpdateScene(SceneManager* sm, double dt)
{
    static float rotValue = 0.0f;
    Transform* trans = sm->GetScene("MainMenuScene")->GetEntity("Zombie")->GetComponent<component::TransformComponent>()->GetTransform();
    trans->SetRotationY(rotValue);
    trans->SetPosition({ 0.0, std::sin(rotValue), 10.0 });

    rotValue += dt;
}