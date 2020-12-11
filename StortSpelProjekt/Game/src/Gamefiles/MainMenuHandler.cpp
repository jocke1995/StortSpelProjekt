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
#include "Misc/EngineRand.h"
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
void onBox(const std::string& name);
void onMouseSensitivityPlus(const std::string& name);
void onMouseSensitivityMinus(const std::string& name);
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
    Texture* mouseSensitivity = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/MouseSensitivity.png");
    Texture* plus = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Plus.png");
    Texture* minus = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Minus.png");
	Texture* music = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/music.png");
	Texture* emptyBox = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/emptybox.png");
	Texture* cross = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/cross.png");
    Texture* background = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Background.png");

	Font* font = al->LoadFontFromFile(L"MedievalSharp.fnt");

    AudioBuffer* menuSound = al->LoadAudio(L"../Vendor/Resources/Audio/Menu.wav", L"MenuMusic");
    menuSound->SetAudioLoop(0);

    // Player (Need a camera)
    Entity* entity = m_pOptionScene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    // Add a voice to the player to play some music.
    vc = entity->AddComponent<component::Audio2DVoiceComponent>();
    vc->AddVoice(L"MenuMusic");

    // Background
    entity = m_pOptionScene->AddEntity("OptionsBackground");
    float2 quadPos = { 0.0f, 0.0f };
    float2 quadScale = { 1.0f, 1.0f };
    float4 notBlended = { 1.0, 1.0, 1.0, 1.0 };
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad(
        "OptionsBackground",
        quadPos, quadScale,
        false, false,
        0,
        notBlended,
        background);

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
        2,
        { 1.0,1.0,1.0,1.0 },
        resolution);
    entity = m_pOptionScene->AddEntity("1444p");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("1444p",
        { 0.35f, 0.15f },
        { (float)res1440p->GetWidth() / 1920.0f, (float)res1440p->GetHeight() / 1080.0f },
        true,
        true,
        2,
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
        2,
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
        2,
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
            1,
            { 1.0,1.0,1.0,1.0 },
            nullptr,
            { 0.30f, 0.20f, 0.25f });
        break;
    case 1080:
        guic->GetQuadManager()->CreateQuad("activeResolution",
            { 0.35f, 0.25f },
            { (float)res1080p->GetWidth() / 1920.0f, (float)res1080p->GetHeight() / 1080.0f },
            false,
            false,
            1,
            { 1.0, 1.0, 1.0, 1.0 },
            nullptr,
            { 0.30f, 0.20f, 0.25f });
        break;
    case 1440:
        guic->GetQuadManager()->CreateQuad("activeResolution",
            { 0.35f, 0.15f },
            { (float)res1440p->GetWidth() / 1920.0f, (float)res1440p->GetHeight() / 1080.0f },
            false,
            false,
            1,
            { 1.0,1.0,1.0,1.0 },
            nullptr,
            { 0.30f, 0.20f, 0.25f });
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
        2,
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
        2,
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
            1,
            { 1.0,1.0,1.0,1.0 },
            nullptr,
            { 0.30f, 0.20f, 0.25f });
        break;
    case 0:
        guic->GetQuadManager()->CreateQuad("activeWindowmode",
            { 0.60f, 0.25f },
            { (float)windowed->GetWidth() / 1920.0f, (float)windowed->GetHeight() / 1080.0f },
            false,
            false,
            1,
            { 1.0, 1.0, 1.0, 1.0 },
            nullptr,
            { 0.30f, 0.20f, 0.25f });
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
        2,
        { 1.0,1.0,1.0,1.0 },
        shadowQuality);

    entity = m_pOptionScene->AddEntity("lowShadowQuality");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("lowShadowQuality",
        { 0.35f, 0.60f },
        { (float)lowShadowQuality->GetWidth() / 1920.0f, (float)lowShadowQuality->GetHeight() / 1080.0f },
        true,
        true,
        2,
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
        2,
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
        2,
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
            1,
            { 1.0,1.0,1.0,1.0 },
            nullptr,
            { 0.30f, 0.20f, 0.25f });
        break;
    case 1:
        guic->GetQuadManager()->CreateQuad("activeShadowQuality",
            { 0.35f, 0.70f },
            { (float)medShadowQuality->GetWidth() / 1920.0f, (float)medShadowQuality->GetHeight() / 1080.0f },
            false,
            false,
            1,
            { 1.0, 1.0, 1.0, 1.0 },
            nullptr,
            { 0.30f, 0.20f, 0.25f });
        break;
    case 2:
        guic->GetQuadManager()->CreateQuad("activeShadowQuality",
            { 0.35f, 0.80f },
            { (float)highShadowQuality->GetWidth() / 1920.0f, (float)highShadowQuality->GetHeight() / 1080.0f },
            false,
            false,
            1,
            { 1.0, 1.0, 1.0, 1.0 },
            nullptr,
            { 0.30f, 0.20f, 0.25f });
        break;
    }
    /*-------------Volume--------------*/
    entity = m_pOptionScene->AddEntity("volumeText");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("volumeText",
        { 0.65f, 0.42f },
        { (float)volume->GetWidth() / 1920.0f, (float)volume->GetHeight() / 1080.0f },
        false,
        false,
        2,
        { 1.0,1.0,1.0,1.0 },
        volume);

    entity = m_pOptionScene->AddEntity("volumePlus");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("volumePlus",
        { 0.66f, 0.60f },
        { (float)plus->GetWidth() / 1920.0f, (float)plus->GetHeight() / 1080.0f },
        true,
        true,
        2,
        { 1.0,1.0,1.0,1.0 },
        plus);
    guic->GetQuadManager()->SetOnClicked(&onVolumePlus);

    entity = m_pOptionScene->AddEntity("volumeMinus");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("volumeMinus",
        { 0.75f, 0.60f },
        { (float)minus->GetWidth() / 1920.0f, (float)minus->GetHeight() / 1080.0f },
        true,
        true,
        2,
        { 1.0,1.0,1.0,1.0 },
        minus);
    guic->GetQuadManager()->SetOnClicked(&onVolumeMinus);

    std::string textToRender = Option::GetInstance().GetVariable("f_volume");
    float2 textPos = { 0.7f, 0.52f };
    float2 textPadding = { 0.5f, 0.0f };
    float4 textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float2 textScale = { 1.0f, 1.0f };
    float4 textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

    entity = m_pOptionScene->AddEntity("volume");
    guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetTextManager()->SetFont(font);
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
        2,
        { 1.0, 1.0, 1.0, 0.0 },
        nullptr
    );

	entity = m_pOptionScene->AddEntity("musicText");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("musicText",
		{ 0.82f, 0.42f },
		{ (float)music->GetWidth() / 1920.0f / 1.8f, (float)music->GetHeight() / 1080.0f / 1.8f},
		false,
		false,
		2,
		{ 1.0,1.0,1.0,1.0 },
		music);

	entity = m_pOptionScene->AddEntity("box");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("emptyBox",
		{ 0.88f, 0.53f },
		{ (float)emptyBox->GetWidth() / 1920.0f / 3.0f, (float)emptyBox->GetHeight() / 1080.0f / 3.0f },
		true,
		true,
		2,
		{ 1.0,1.0,1.0,1.0 },
		emptyBox);
	guic->GetQuadManager()->SetOnClicked(&onBox);

	bool active = std::atof(Option::GetInstance().GetVariable("i_music").c_str());
	float4 blended = { 1.0, 1.0, 1.0, 1.0 };
	if (!active)
	{
		blended.w = 0.0;
	}
	entity = m_pOptionScene->AddEntity("check");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("check",
		{ 0.88f, 0.53f },
		{ (float)emptyBox->GetWidth() / 1920.0f / 3.0f, (float)emptyBox->GetHeight() / 1080.0f / 3.0f },
		false,
		false,
		2,
		blended,
		cross);

    /*-------------Mouse Sensitivity--------------*/
    entity = m_pOptionScene->AddEntity("MouseSensitivityText");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("MouseSensitivityText",
        { 0.65f, 0.72f },
        { (float)mouseSensitivity->GetWidth() / 1920.0f, (float)mouseSensitivity->GetHeight() / 1080.0f },
        false,
        false,
        2,
        { 1.0,1.0,1.0,1.0 },
        mouseSensitivity);

    entity = m_pOptionScene->AddEntity("MouseSensitivityPlus");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("MouseSensitivityPlus",
        { 0.66f, 0.90f },
        { (float)plus->GetWidth() / 1920.0f, (float)plus->GetHeight() / 1080.0f },
        true,
        true,
        2,
        { 1.0,1.0,1.0,1.0 },
        plus);
    guic->GetQuadManager()->SetOnClicked(&onMouseSensitivityPlus);

    entity = m_pOptionScene->AddEntity("MouseSensitivityMinus");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("MouseSensitivityMinus",
        { 0.75f, 0.90f },
        { (float)minus->GetWidth() / 1920.0f, (float)minus->GetHeight() / 1080.0f },
        true,
        true,
        2,
        { 1.0,1.0,1.0,1.0 },
        minus);
    guic->GetQuadManager()->SetOnClicked(&onMouseSensitivityMinus);

    textToRender = Option::GetInstance().GetVariable("f_sensitivityX");
    textPos = { 0.7f, 0.82f };
    textPadding = { 0.5f, 0.0f };
    textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    textScale = { 1.0f, 1.0f };
    textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

    entity = m_pOptionScene->AddEntity("MouseSensitivity");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetTextManager()->SetFont(font);
    guic->GetTextManager()->AddText("MouseSensitivity");
    guic->GetTextManager()->SetColor(textColor, "MouseSensitivity");
    guic->GetTextManager()->SetPadding(textPadding, "MouseSensitivity");
    guic->GetTextManager()->SetPos(textPos, "MouseSensitivity");
    guic->GetTextManager()->SetScale(textScale, "MouseSensitivity");
    guic->GetTextManager()->SetText(textToRender, "MouseSensitivity");
    guic->GetTextManager()->SetBlend(textBlend, "MouseSensitivity");

    //guic->GetQuadManager()->CreateQuad(
    //    "money",
    //    { 0.7f, 0.65f }, { 0.1f, 0.1f },
    //    false, false,
    //    1,
    //    { 1.0, 1.0, 1.0, 0.0 },
    //    nullptr
    //);



    /*-------------Back--------------*/
    entity = m_pOptionScene->AddEntity("restartWarning");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("restartWarning",
        { 0.03f, 0.55f },
        { (float)restart->GetWidth() / 1920.0f, (float)restart->GetHeight() / 1080.0f },
        false,
        false,
        2,
        { 1.0,1.0,1.0,1.0 },
        restart);


    entity = m_pOptionScene->AddEntity("Back");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("Back",
        { 0.05f, 0.8f },
        { (float)exitTex->GetWidth() / 1920.0f, (float)exitTex->GetHeight() / 1080.0f },
        true,
        true,
        2,
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
    EngineRand rand;
    rand.SetSeed(time(0));

    AssetLoader* al = AssetLoader::Get();

    Scene* scene = sm->CreateScene("MainMenuScene");

    component::GUI2DComponent* guic = nullptr;
    component::Audio2DVoiceComponent* vc = nullptr;

    Texture* startTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Start.png");
    Texture* optionsTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Options.png");
    Texture* exitTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Exit.png");
	Texture* background = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Background.png");
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/IgnoredModels/Player/AnimatedPlayer.fbx");

    AudioBuffer* menuSound = al->LoadAudio(L"../Vendor/Resources/Audio/Menu.wav", L"MenuMusic");
    menuSound->SetAudioLoop(0);

    // Player (Need a camera)
    Entity* entity = scene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    // Add a voice to the player to play some music.
    vc = entity->AddComponent<component::Audio2DVoiceComponent>();
    vc->AddVoice(L"MenuMusic");

    component::ModelComponent* mc = entity->AddComponent<component::ModelComponent>();
    component::TransformComponent* tc = entity->AddComponent<component::TransformComponent>();
    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    double3 playerDim = mc->GetModelDim();
    Transform* t = tc->GetTransform();
    t->SetPosition(0.0f, 0.0f, 10.0f);
    t->SetScale(2.0 / playerDim.y);
    t->SetRotationX(0.0);
    t->SetRotationY(PI);
    t->SetRotationZ(0.0);
    tc->SetTransformOriginalState();
    
    
    double rad = playerDim.z / 2.0;
    double cylHeight = playerDim.y - (rad * 2.0);
    component::CollisionComponent* cc = entity->AddComponent<component::CapsuleCollisionComponent>(200.0, rad, cylHeight, 0.0, 0.0, false);
    cc->SetGravity(0.0);

	// Background
	entity = scene->AddEntity("background");
	float2 quadPos = { 0.0f, 0.0f };
	float2 quadScale = { 1.0f, 1.0f };
	float4 notBlended = { 1.0, 1.0, 1.0, 1.0 };
	component::GUI2DComponent* gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetQuadManager()->CreateQuad(
		"background",
		quadPos, quadScale,
		false, false,
		0,
		notBlended,
		background);

	// Skybox
	entity = scene->AddEntity("skybox");
	component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
	TextureCubeMap* blackCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/black.dds");
	sbc->SetTexture(blackCubeMap);

	// Since the target platform uses 1920 : 1080 ratio we use this same ratio for our texture sizes.

	entity = scene->AddEntity("StartOption");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("StartOption", { 0.1f, 0.1f }, { startTex->GetWidth() / 1920.0f, startTex->GetHeight() / 1080.0f }, true, true, 2, { 1.0,1.0,1.0,1.0 }, startTex);
	guic->GetQuadManager()->SetOnClicked(&onStart);

	entity = scene->AddEntity("OptionsOption");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("OptionsOption", { 0.1f, 0.25f }, { optionsTex->GetWidth() / 1920.0f, optionsTex->GetHeight() / 1080.0f }, true, true, 2, { 1.0,1.0,1.0,1.0 }, optionsTex);
	guic->GetQuadManager()->SetOnClicked(&onOptions);

	entity = scene->AddEntity("ExitOption");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("ExitOption", { 0.1f, 0.4f }, { exitTex->GetWidth() / 1920.0f, exitTex->GetHeight() / 1080.0f }, true, true, 2, { 1.0,1.0,1.0,1.0 }, exitTex);
	guic->GetQuadManager()->SetOnClicked(&onExit);

	//std::vector<Model*> enemyModels;
	//enemyModels.push_back(al->LoadModel(L"../Vendor/Resources/Models/Zombie/zombie.obj"));
	//enemyModels.push_back(al->LoadModel(L"../Vendor/Resources/Models/IgnoredModels/Demon/AnimatedDemon.fbx"));
    //for (int i = 0; i < 20; ++i)
    //{
    //    entity = scene->AddEntity("menuEnemy" + std::to_string(i));
    //    component::ModelComponent* mc = entity->AddComponent<component::ModelComponent>();
    //    component::TransformComponent* tc = entity->AddComponent<component::TransformComponent>();
    //    int enemyModel = rand.Rand(0, enemyModels.size());
    //    mc->SetModel(enemyModels.at(enemyModel));
    //    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    //    double3 enemyDim = mc->GetModelDim();
    //    Transform* t = tc->GetTransform();
    //    t->SetPosition(rand.Randf(0.0, 20.0) - 10.0f, 0.0, rand.Randf(0.0, 20.0));
    //    t->SetScale(2.0 / enemyDim.y);
    //    t->SetRotationX(0.0);
    //    t->SetRotationY(PI);
    //    t->SetRotationZ(0.0);
    //    tc->SetTransformOriginalState();
	//
	//
    //    double rad = enemyDim.z / 2.0;
    //    double cylHeight = enemyDim.y - (rad * 2.0);
    //    component::CollisionComponent* cc = entity->AddComponent<component::CapsuleCollisionComponent>(200.0, rad, cylHeight, 0.0, 0.0, false);
    //    cc->SetGravity(0.0);
    //}
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
        { 0.30f, 0.20f, 0.25f });
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
        { 0.30f, 0.20f, 0.25f });
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
        { 0.30f, 0.20f, 0.25f });
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
        { 0.30f, 0.20f, 0.25f });
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
        { 0.30f, 0.20f, 0.25f });
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
        { 0.30f, 0.20f, 0.25f });
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
        { 0.30f, 0.20f, 0.25f });
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
        { 0.30f, 0.20f, 0.25f });
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

void onBox(const std::string& name)
{	
	bool active = std::atof(Option::GetInstance().GetVariable("i_music").c_str());
	Option::GetInstance().SetVariable("i_music", std::to_string(!active));
	Option::GetInstance().WriteFile();

	float4 blended = { 1.0, 1.0, 1.0, 1.0 };
	if (active)
	{
		blended.w = 0.0;
	}

	Texture* emptyBox = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/emptybox.png");
	MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("check")->GetComponent<component::GUI2DComponent>()->GetQuadManager()->UpdateQuad
	(
		{ 0.88f, 0.53f },
		{ (float)emptyBox->GetWidth() / 1920.0f / 3.0f, (float)emptyBox->GetHeight() / 1080.0f / 3.0f },
		false,
		false,
		blended
	);
}

void onMouseSensitivityPlus(const std::string& name)
{
    if (std::stof(Option::GetInstance().GetVariable("f_sensitivityX")) < 10)
    {
        std::ostringstream str;

        // X
        str << std::setprecision(2) << std::stof(Option::GetInstance().GetVariable("f_sensitivityX")) + 0.1f;
        Option::GetInstance().SetVariable("f_sensitivityX", str.str());

        // Y
        str.str("");
        str << std::setprecision(2) << std::stof(Option::GetInstance().GetVariable("f_sensitivityY")) + 0.1f;
        Option::GetInstance().SetVariable("f_sensitivityY", str.str());

        Option::GetInstance().WriteFile();
    }
}

void onMouseSensitivityMinus(const std::string& name)
{
    if (std::stof(Option::GetInstance().GetVariable("f_sensitivityX")) > 0)
    {
        std::ostringstream str;

        // X
        str << std::setprecision(2) << std::stof(Option::GetInstance().GetVariable("f_sensitivityX")) - 0.1f;
        Option::GetInstance().SetVariable("f_sensitivityX", str.str());

        // Y
        str.str("");
        str << std::setprecision(2) << std::stof(Option::GetInstance().GetVariable("f_sensitivityY")) - 0.1f;
        Option::GetInstance().SetVariable("f_sensitivityY", str.str());

        Option::GetInstance().WriteFile();
    }
}


void MainMenuUpdateScene(SceneManager* sm, double dt)
{
    //static float rotValue = 0.0f;
	//
    //Transform* trans = sm->GetScene("MainMenuScene")->GetEntity("player")->GetComponent<component::TransformComponent>()->GetTransform();
    //trans->SetRotationY(rotValue);
    //trans->SetPosition({ 0.0f, std::sin(rotValue), 10.0f });
	//
    //for (int i = 0; i < 20; ++i)
    //{
    //    trans = sm->GetScene("MainMenuScene")->GetEntity("menuEnemy" + std::to_string(i))->GetComponent<component::TransformComponent>()->GetTransform();
    //    trans->SetRotationY(rotValue + i);
    //    float3 pos = trans->GetPositionFloat3();
    //    trans->SetPosition({ pos.x, std::sin(rotValue + i), pos.z });
    //}
	//
    //rotValue += dt;
}