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
void onBrightnessPlus(const std::string& name);
void onBrightnessMinus(const std::string& name);
void onStart(const std::string& name);
void onExit(const std::string& name);
void onCredits(const std::string& name);
void onOptions(const std::string& name);
void onOptionBack(const std::string& name);
void onHowToPlay(const std::string& name);
void onHowToPlayBack(const std::string& name);
void onControlls(const std::string& name);
void onControllsBack(const std::string& name);
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
void CreditsUpdateScene(SceneManager* sm, double dt);
void ResetCreditsScene(SceneManager* sm);
void MenuUpdateScene(SceneManager* sm, double dt);

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
	Texture* brightness = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Brightness.png");

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

	/*------------Brightness------------*/
	entity = m_pOptionScene->AddEntity("BrightnessText");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("BrightnessText",
		{ 0.05f, 0.05f },
		{ (float)resolution->GetWidth() / 1920.0f, (float)resolution->GetHeight() / 1080.0f },
		false,
		false,
		2,
		{ 1.0,1.0,1.0,1.0 },
		brightness);

	entity = m_pOptionScene->AddEntity("BrightnessPlus");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("BrightnessPlus",
		{ 0.06f, 0.23f },
		{ (float)plus->GetWidth() / 1920.0f, (float)plus->GetHeight() / 1080.0f },
		true,
		true,
		2,
		{ 1.0,1.0,1.0,1.0 },
		plus);
	guic->GetQuadManager()->SetOnClicked(&onBrightnessPlus);

	entity = m_pOptionScene->AddEntity("BrightnessMinus");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("BrightnessMinus",
		{ 0.15f, 0.23f },
		{ (float)minus->GetWidth() / 1920.0f, (float)minus->GetHeight() / 1080.0f },
		true,
		true,
		2,
		{ 1.0,1.0,1.0,1.0 },
		minus);
	guic->GetQuadManager()->SetOnClicked(&onBrightnessMinus);

	std::string textToRender = Option::GetInstance().GetVariable("f_brightness");
	float2 textPos = { 0.10f, 0.16f };
	float2 textPadding = { 0.5f, 0.0f };
	float4 textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float2 textScale = { 1.0f, 1.0f };
	float4 textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

	entity = m_pOptionScene->AddEntity("brightness");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetTextManager()->SetFont(font);
	guic->GetTextManager()->AddText("brightness");
	guic->GetTextManager()->SetColor(textColor, "brightness");
	guic->GetTextManager()->SetPadding(textPadding, "brightness");
	guic->GetTextManager()->SetPos(textPos, "brightness");
	guic->GetTextManager()->SetScale(textScale, "brightness");
	guic->GetTextManager()->SetText(textToRender, "brightness");
	guic->GetTextManager()->SetBlend(textBlend, "brightness");

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

    textToRender = Option::GetInstance().GetVariable("f_masterVolume");
    textPos = { 0.7f, 0.52f };
    textPadding = { 0.5f, 0.0f };
    textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    textScale = { 1.0f, 1.0f };
    textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

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

    m_pOptionScene->SetUpdateScene(&MenuUpdateScene);
}

void MainMenuHandler::createHowToPlayScene()
{
    AssetLoader* al = AssetLoader::Get();

    m_pHowToPlayScene = m_pSceneManager->CreateScene("HowToPlayScene");

    component::GUI2DComponent* guic = nullptr;
    component::Audio2DVoiceComponent* vc = nullptr;

    Texture* exitTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Back.png");
    Texture* mouse = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HowToPlay/mouseArrow.png");
    Texture* buttons = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HowToPlay/buttonsArrow.png");
    Texture* controller = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HowToPlay/Controller.png");
    Texture* controls = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HowToPlay/Controls.png");
    Texture* background = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Background.png");

    Font* font = al->LoadFontFromFile(L"MedievalSharp.fnt");

    AudioBuffer* menuSound = al->LoadAudio(L"../Vendor/Resources/Audio/Menu.wav", L"MenuMusic");
    menuSound->SetAudioLoop(0);

    // Player (Need a camera)
    Entity* entity = m_pHowToPlayScene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    // Add a voice to the player to play some music.
    vc = entity->AddComponent<component::Audio2DVoiceComponent>();
    vc->AddVoice(L"MenuMusic");

    // Background
    entity = m_pHowToPlayScene->AddEntity("OptionsBackground");
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
    entity = m_pHowToPlayScene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    TextureCubeMap* blackCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/black.dds");
    sbc->SetTexture(blackCubeMap);


    entity = m_pHowToPlayScene->AddEntity("Buttons");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad(
        "Buttons",
        { 0.15f, 0.5f },
        { (float)((float)buttons->GetWidth() / 1920.0f ) / 1.5f, (float)((float)buttons->GetHeight() / 1080.0f ) / 1.5f},
        false,
        false,
        2,
        { 1.0,1.0,1.0,1.0 },
        buttons);

    entity = m_pHowToPlayScene->AddEntity("Mouse");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad(
        "Mouse",
        { 0.3f, 0.53f },
        { (float)((float)buttons->GetWidth() / 1920.0f) / 2.2f, (float)((float)buttons->GetHeight() / 1080.0f) / 2.2f },
        false,
        false,
        2,
        { 1.0,1.0,1.0,1.0 },
        mouse);

    entity = m_pHowToPlayScene->AddEntity("Controller");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad(
        "Controller",
        { 0.6f, 0.5f },
        { (float)((float)controller->GetWidth() / 1920.0f) / 1.f, (float)((float)controller->GetHeight() / 1080.0f) / 1.f },
        false,
        false,
        2,
        { 1.0,1.0,1.0,1.0 },
        controller);

    entity = m_pHowToPlayScene->AddEntity("Controls");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad(
        "Controls",
        { 0.35f, 0.15f },
        { (float)((float)controls->GetWidth() / 1920.0f) / 1.f, (float)((float)controls->GetHeight() / 1080.0f) / 1.f },
        false,
        false,
        2,
        { 1.0,1.0,1.0,1.0 },
        controls);    

    /*-------------Back--------------*/
   
    entity = m_pHowToPlayScene->AddEntity("Back");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("Back",
        { 0.05f, 0.8f },
        { (float)exitTex->GetWidth() / 1920.0f, (float)exitTex->GetHeight() / 1080.0f },
        true,
        true,
        2,
        { 1.0,1.0,1.0,1.0 },
        exitTex);
    guic->GetQuadManager()->SetOnClicked(&onHowToPlayBack);

    m_pHowToPlayScene->SetUpdateScene(&MenuUpdateScene);
}

void MainMenuHandler::createCreditsScene()
{
    AssetLoader* al = AssetLoader::Get();

    m_pCreditsScene = m_pSceneManager->CreateScene("CreditsScene");
    m_pCreditsScene->SetUpdateScene(&CreditsUpdateScene);

    component::GUI2DComponent* guic = nullptr;
    component::Audio2DVoiceComponent* vc = nullptr;

    Texture* background = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Background_credits.png");
    Texture* exitTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Back.png");

    Font* font = al->LoadFontFromFile(L"MedievalSharp.fnt");

    AudioBuffer* menuSound = al->LoadAudio(L"../Vendor/Resources/Audio/Menu.wav", L"MenuMusic");
    menuSound->SetAudioLoop(0);

    // Player (Need a camera)
    Entity* entity = m_pCreditsScene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    // Add a voice to the player to play some music.
    vc = entity->AddComponent<component::Audio2DVoiceComponent>();
    vc->AddVoice(L"MenuMusic");

    // Background
    entity = m_pCreditsScene->AddEntity("CreditsBackground");
    float2 quadPos = { 0.0f, 0.0f };
    float2 quadScale = { 1.0f, 1.0f };
    float4 notBlended = { 1.0, 1.0, 1.0, 1.0 };
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad(
        "CreditsBackground",
        quadPos, quadScale,
        false, false,
        0,
        notBlended,
        background);

    // Skybox
    entity = m_pCreditsScene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    TextureCubeMap* blackCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/black.dds");
    sbc->SetTexture(blackCubeMap);





    /////////////////////////////////////////////
    // Scrolling Text
    /////////////////////////////////////////////

    entity = m_pCreditsScene->AddEntity("ScrollingText");
    guic = entity->AddComponent<component::GUI2DComponent>();

    float x_leftmost = 0.20;
    float x_author = 0.61;

    float y = 1;
    float y_big = 0.3;
    float y_after_big = 0.1;
    float y_small = 0.06;

    float size_big = 1.15;
    float size_medium = 0.8;
    float size_small = 0.5;

    std::string name = "";



    std::string ournames[10];
    ournames[0] = "Andrés Diaz";
    ournames[1] = "Anton Åsbrink";
    ournames[2] = "Björn Johansson";
    ournames[3] = "Filip Zachrisson";
    ournames[4] = "Fredrik Lind";
    ournames[5] = "Jacob Andersson";
    ournames[6] = "Joakim Sjöberg";
    ournames[7] = "Leo Wikström";
    ournames[8] = "Tim Johansson";
    ournames[9] = "William Osberg Resin";

    std::string boughtPackName = "";

	guic->GetTextManager()->SetFont(font);

    // Hell Loop
    name = "hellloop";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Hell Loop", name);
    guic->GetTextManager()->SetPos({ 0.415, y }, name);
    guic->GetTextManager()->SetScale({ size_big, size_big }, name);






    // project
    name = "project";
    y += y_big;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("This game is a university project made during 2020-09-01 to 2020-12-18", name);
    guic->GetTextManager()->SetPos({ 0.18, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);






    // Programmers
    name = "programmers";
    y += y_big;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Programmers", name);
    guic->GetTextManager()->SetPos({ 0.36, y }, name);
    guic->GetTextManager()->SetScale({ size_big, size_big }, name);

    // ournames_0
    name = "ournames_0";
    y += y_after_big;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText( ournames[0], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // ournames_1
    name = "ournames_1";
    y += y_small;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[1], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // ournames_2
    name = "ournames_2";
    y += y_small;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[2], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // ournames_3
    name = "ournames_3";
    y += y_small;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[3], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // ournames_4
    name = "ournames_4";
    y += y_small;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[4], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // ournames_5
    name = "ournames_5";
    y += y_small;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[5], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // ournames_6
    name = "ournames_6";
    y += y_small;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[6], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // ournames_7
    name = "ournames_7";
    y += y_small;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[7], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // ournames_8
    name = "ournames_8";
    y += y_small;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[8], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // ournames_9
    name = "ournames_9";
    y += y_small;
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[9], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);




    // rendering
    y += y_big;
    name = "rendering";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Rendering Programmers", name);
    guic->GetTextManager()->SetPos({ 0.25, y }, name);
    guic->GetTextManager()->SetScale({ size_big, size_big }, name);

    // rendering_0
    y += y_after_big;
    name = "rendering_0";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[3], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // rendering_1
    y += y_small;
    name = "rendering_1";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[4], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // rendering_2
    y += y_small;
    name = "rendering_2";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[6], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);



    // gamedesign
    y += y_big;
    name = "gamedesign";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Game Design", name);
    guic->GetTextManager()->SetPos({ 0.37, y }, name);
    guic->GetTextManager()->SetScale({ size_big, size_big }, name);

    // gamedesign_0
    y += y_after_big;
    name = "gamedesign_0";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[1], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // gamedesign_1
    y += y_small;
    name = "gamedesign_1";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[2], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // gamedesign_2
    y += y_small;
    name = "gamedesign_2";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[4], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // gamedesign_3
    y += y_small;
    name = "gamedesign_3";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[9], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);






    // audio
    y += y_big;
    name = "audio";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Audio", name);
    guic->GetTextManager()->SetPos({ 0.445, y }, name);
    guic->GetTextManager()->SetScale({ size_big, size_big }, name);

    // audio_0
    y += y_after_big;
    name = "audio_0";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[0], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // audio_1
    y += y_small;
    name = "audio_1";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[5], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // audio_2
    y += y_small;
    name = "audio_2";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[8], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);






    // art
    y += y_big;
    name = "art";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Art", name);
    guic->GetTextManager()->SetPos({ 0.47, y }, name);
    guic->GetTextManager()->SetScale({ size_big, size_big }, name);

    // art_0
    y += y_after_big;
    name = "art_0";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[1], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // art_1
    y += y_small;
    name = "art_1";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[3], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // art_2
    y += y_small;
    name = "art_2";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[4], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);







    // leveldesign
    y += y_big;
    name = "leveldesign";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Level Design", name);
    guic->GetTextManager()->SetPos({ 0.38, y }, name);
    guic->GetTextManager()->SetScale({ size_big, size_big }, name);

    // leveldesign_0
    y += y_after_big;
    name = "leveldesign_0";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText(ournames[4], name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);








    // specialthanks
    y += y_big;
    name = "specialthanks";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Special Thanks", name);
    guic->GetTextManager()->SetPos({ 0.36, y }, name);
    guic->GetTextManager()->SetScale({ size_big, size_big }, name);

    // specialthanks_2
    y += y_after_big;
    name = "specialthanks_0";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Erik Wadstein", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // specialthanks_0
    y += y_small;
    name = "specialthanks_1";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Hans Tap", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // specialthanks_1
    y += y_small;
    name = "specialthanks_2";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Stefan Petersson", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    // specialthanks_3
    y += y_small;
    name = "specialthanks_3";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Blekinge Institute of Technology", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);








    // thirdparty
    y += y_big;
    name = "thirdparty";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Third Party", name);
    guic->GetTextManager()->SetPos({ 0.385, y }, name);
    guic->GetTextManager()->SetScale({ size_big, size_big }, name);


/*

Sorted

Anvil:                 seenoise @Sketchfab
Brazier:               persnetto @Sketchfab
Chandelier:            Kevin.Popescu @Sketchfab
Demon:                 Infinity PBR
Outdoor floor:         ErenKatsukagi @Twitter
Palisade:              adam127 @Sketchfab
Pebble texture:        ErenKatsukagi @Twitter
Player:                Infinity PBR
Shop goblin:           Batuhan13 @Sketchfab
Skulls:                Abimael Gonzalez @Sketchfab
Spider:                Infinity PBR
Statue:                Arodin @Sketchfab
Stone walls:           ErenKatsukagi @Twitter
Sword in pile:         LowSeb @Sketchfab
Tomb:                  persnetto @Sketchfab
Torch:                 Kigha @Sketchfab
Tree:                  v124entkl @Sketchfab
Wooden barrel:         BatuhanOZER @Turbosquid
Zombie:                cypler @Sketchfab

*/

    // thirdparty_models
    y += y_after_big;
    name = "thirdparty_models";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Models", name);
    guic->GetTextManager()->SetPos({ 0.45, y }, name);
    guic->GetTextManager()->SetScale({ size_medium, size_medium }, name);

    y += y_after_big;
    name = "thirdparty_models_0";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Anvil:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_0§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("seenoise @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_01";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Brazier:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_01§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("persnetto @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_1";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Chandelier:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_1§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Kevin.Popescu @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_1100";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Demon:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_1100§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Infinity PBR", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_2";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Outdoor floor:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_2§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("ErenKatsukagi @Twitter", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_3";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Palisade:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_3§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("adam127 @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_4";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Pebble texture:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_4§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("ErenKatsukagi @Twitter", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_5";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Player:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_5§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Infinity PBR", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_6";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Shop goblin:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_6§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Batuhan13 @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_7";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Skulls:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_7§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Abimael Gonzalez @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_71";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Spider:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_71§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Infinity PBR", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_8";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Statue:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_8§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Arodin @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_9";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Stone walls:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_9§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("ErenKatsukagi @Twitter", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_10";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Sword in pile:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_10§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("LowSeb @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_11";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Tomb:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_11§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("persnetto @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_111";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Torch:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_111§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Kigha @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_12";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Tree:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_12§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("v124entkl @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_13";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Wooden barrel:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_13§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("BatuhanOZER @Turbosquid", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_models_14";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Zombie:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_models_14§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("cypler @Sketchfab", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);


/*

Sorted

Ingame ambient sound:  phlair @freesound
Ingame music:          mrrap4food @freesound
Melee attack:          Zapsplat
Player sounds:         SkyRaeVoicing @freesound
Range attack:          Zapsplat
Shop music:            Darkash28 @freesound
Spider attack:         Infinity PBR
Spider crawl:          dasrealized @freesound
Zombie Sounds:         Infinity PBR

*/

    // thirdparty_sounds
    y += y_big;
    name = "thirdparty_sounds";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Sounds", name);
    guic->GetTextManager()->SetPos({ 0.45, y }, name);
    guic->GetTextManager()->SetScale({ size_medium, size_medium }, name);

    y += y_after_big;
    name = "thirdparty_sounds_0";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Ingame ambient sound: ", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_sounds_0§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("phlair @freesound", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_sounds_01";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Ingame music:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_sounds_01§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("mrrap4food @freesound", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_sounds_1";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Melee attack:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_sounds_1§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Zapsplat", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_sounds_2";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Player sounds:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_sounds_2§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("SkyRaeVoicing @freesound", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_sounds_3";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Range attack:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_sounds_3§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Zapsplat", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_sounds_31";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Shop music:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_sounds_31§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Darkash28 @freesound", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_sounds_4";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Spider attack:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_sounds_4§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Infinity PBR", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_sounds_5";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Spider crawl:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_sounds_5§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("dasrealized @freesound", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    y += y_small;
    name = "thirdparty_sounds_6";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Zombie Sounds:", name);
    guic->GetTextManager()->SetPos({ x_leftmost, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);

    name = "thirdparty_sounds_6§";
    guic->GetTextManager()->AddText(name);
    guic->GetTextManager()->SetText("Infinity PBR", name);
    guic->GetTextManager()->SetPos({ x_author, y }, name);
    guic->GetTextManager()->SetScale({ size_small, size_small }, name);


    /////////////////////////////////////////////
    // Exit
    /////////////////////////////////////////////

    entity = m_pCreditsScene->AddEntity("Back");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("Back",
        { 0.05f, 0.8f },
        { (float)exitTex->GetWidth() / 1920.0f, (float)exitTex->GetHeight() / 1080.0f },
        true,
        true,
        2,
        { 1.0,1.0,1.0,1.0 },
        exitTex);
    guic->GetQuadManager()->SetOnClicked(&onOptionBack); // back to same scene as Options Back

    // --------------------------------------------------------------
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
    Texture* howToPlayTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HowToPlay/HowToPlay.png");
    Texture* exitTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Exit.png");
    Texture* creditsTex = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Credits.png");
	Texture* background = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Background.png");
	Texture* title = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/title.png");
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

	// Title
	entity = scene->AddEntity("Title");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("title",
		{ 0.08f, 0.05f },
		{ (float)title->GetWidth() / 1920.0f / 1.15f, (float)title->GetHeight() / 1080.0f / 1.2f},
		false,
		false,
		1,
		{ 1.0,1.0,1.0,1.0 },
		title);

	// Skybox
	entity = scene->AddEntity("skybox");
	component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
	TextureCubeMap* blackCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/black.dds");
	sbc->SetTexture(blackCubeMap);

	// Since the target platform uses 1920 : 1080 ratio we use this same ratio for our texture sizes.

	entity = scene->AddEntity("StartOption");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("StartOption", { 0.1f, 0.35f }, { startTex->GetWidth() / 1920.0f, startTex->GetHeight() / 1080.0f }, true, true, 2, { 1.0,1.0,1.0,1.0 }, startTex);
	guic->GetQuadManager()->SetOnClicked(&onStart);

	entity = scene->AddEntity("OptionsOption");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("OptionsOption", { 0.1f, 0.5f }, { optionsTex->GetWidth() / 1920.0f, optionsTex->GetHeight() / 1080.0f }, true, true, 2, { 1.0,1.0,1.0,1.0 }, optionsTex);
	guic->GetQuadManager()->SetOnClicked(&onOptions);

	entity = scene->AddEntity("ExitOption");
	guic = entity->AddComponent<component::GUI2DComponent>();
	guic->GetQuadManager()->CreateQuad("ExitOption", { 0.1f, 0.8f }, { exitTex->GetWidth() / 1920.0f, exitTex->GetHeight() / 1080.0f }, true, true, 2, { 1.0,1.0,1.0,1.0 }, exitTex);
	guic->GetQuadManager()->SetOnClicked(&onExit);

    entity = scene->AddEntity("HowToPlayOption");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("HowToPlayOption", { 0.1f, 0.64f }, { howToPlayTex->GetWidth() / 1920.0f, howToPlayTex->GetHeight() / 1080.0f }, true, true, 2, { 1.0,1.0,1.0,1.0 }, howToPlayTex);
    guic->GetQuadManager()->SetOnClicked(&onHowToPlay);
	
    entity = scene->AddEntity("CreditsOption");
    guic = entity->AddComponent<component::GUI2DComponent>();
    guic->GetQuadManager()->CreateQuad("CreditsOption", { 0.67f, 0.802f }, { creditsTex->GetWidth() / 1920.0f, creditsTex->GetHeight() / 1080.0f }, true, true, 2, { 1.0,1.0,1.0,1.0 }, creditsTex);
    guic->GetQuadManager()->SetOnClicked(&onCredits);

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
    createHowToPlayScene();
    createCreditsScene();

    scene->SetUpdateScene(&MainMenuUpdateScene);

    return scene;
}

Scene* MainMenuHandler::GetScene()
{
    return m_pScene;
}

void onMainMenuSceneInit(Scene* scene)
{
    if (std::atof(Option::GetInstance().GetVariable("i_music").c_str()))
    {
        scene->GetEntity("player")->GetComponent<component::Audio2DVoiceComponent>()->Play(L"MenuMusic");
    }
}

void onBrightnessPlus(const std::string& name)
{
	if (std::stof(Option::GetInstance().GetVariable("f_brightness")) < 5.0f)
	{
		std::ostringstream str;
		str << std::fixed << std::setprecision(1) << std::stof(Option::GetInstance().GetVariable("f_brightness")) + 0.1f;
		Option::GetInstance().SetVariable("f_brightness", str.str());

		Option::GetInstance().WriteFile();
	}
}

void onBrightnessMinus(const std::string& name)
{
	if (std::stof(Option::GetInstance().GetVariable("f_brightness")) > 0.0f)
	{
		std::ostringstream str;
		str << std::fixed << std::setprecision(1) << std::stof(Option::GetInstance().GetVariable("f_brightness")) - 0.1f;
		Option::GetInstance().SetVariable("f_brightness", str.str());

		Option::GetInstance().WriteFile();
	}
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

void onCredits(const std::string& name)
{
    ResetCreditsScene(&SceneManager::GetInstance());
    EventBus::GetInstance().Publish(&SceneChange("CreditsScene"));
}

void onOptions(const std::string& name)
{
    EventBus::GetInstance().Publish(&SceneChange("OptionScene"));
}

void onOptionBack(const std::string& name)
{
    EventBus::GetInstance().Publish(&SceneChange("MainMenuScene"));
}

void onHowToPlay(const std::string& name)
{
    EventBus::GetInstance().Publish(&SceneChange("HowToPlayScene"));
}

void onHowToPlayBack(const std::string& name)
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

	Entity* entity = MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeResolution");
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetPos({ 0.35f, 0.15f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetSize({ (float)high->GetWidth() / 1920.0f, (float)high->GetHeight() / 1080.0f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetColor({ 0.30f, 0.20f, 0.25f });
}

void on1920x1080(const std::string& name)
{
    Option::GetInstance().SetVariable("i_resolutionWidth", "1920");
    Option::GetInstance().SetVariable("i_resolutionHeight", "1080");

    Option::GetInstance().SetVariable("i_windowWidth", "1920");
    Option::GetInstance().SetVariable("i_windowHeight", "1080");

    Option::GetInstance().WriteFile();

    Texture* med = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/1920x1080.png");

	Entity* entity = MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeResolution"); 
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetPos({ 0.35f, 0.25f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetSize({ (float)med->GetWidth() / 1920.0f, (float)med->GetHeight() / 1080.0f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetColor({ 0.30f, 0.20f, 0.25f });
}

void on1280x720(const std::string& name)
{
    Option::GetInstance().SetVariable("i_resolutionWidth", "1280");
    Option::GetInstance().SetVariable("i_resolutionHeight", "720");

    Option::GetInstance().SetVariable("i_windowWidth", "1280");
    Option::GetInstance().SetVariable("i_windowHeight", "720");

    Option::GetInstance().WriteFile();

    Texture* low = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/1280x720.png");

	Entity* entity = MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeResolution");
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetPos({ 0.35f, 0.35f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetSize({ (float)low->GetWidth() / 1920.0f, (float)low->GetHeight() / 1080.0f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetColor({ 0.30f, 0.20f, 0.25f });
}

void onFullscreen(const std::string& name)
{
    Option::GetInstance().SetVariable("i_windowMode", "1");

    Option::GetInstance().WriteFile();

    Texture* fullscreen = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Fullscreen.png");

	Entity* entity = MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeWindowmode");
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetPos({ 0.60f, 0.15f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetSize({ (float)fullscreen->GetWidth() / 1920.0f, (float)fullscreen->GetHeight() / 1080.0f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetColor({ 0.30f, 0.20f, 0.25f });
}

void onWindowed(const std::string& name)
{
    Option::GetInstance().SetVariable("i_windowMode", "0");

    Option::GetInstance().WriteFile();

    Texture* window = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Windowed.png");

	Entity* entity = MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeWindowmode");
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetPos({ 0.60f, 0.25f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetSize({ (float)window->GetWidth() / 1920.0f, (float)window->GetHeight() / 1080.0f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetColor({ 0.30f, 0.20f, 0.25f });
}

void onLowShadowQuality(const std::string& name)
{
    Option::GetInstance().SetVariable("i_shadowResolution", "0");

    Option::GetInstance().WriteFile();

    Texture* low = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Low.png");

	Entity* entity = MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeShadowQuality");
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetPos({ 0.35f, 0.60f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetSize({ (float)low->GetWidth() / 1920.0f, (float)low->GetHeight() / 1080.0f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetColor({ 0.30f, 0.20f, 0.25f });
}

void onMedShadowQuality(const std::string& name)
{
    Option::GetInstance().SetVariable("i_shadowResolution", "1");

    Option::GetInstance().WriteFile();

    Texture* med = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Med.png");

	Entity* entity = MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeShadowQuality");
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetPos({ 0.35f, 0.70f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetSize({ (float)med->GetWidth() / 1920.0f, (float)med->GetHeight() / 1080.0f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetColor({ 0.30f, 0.20f, 0.25f });
}

void onHighShadowQuality(const std::string& name)
{
    Option::GetInstance().SetVariable("i_shadowResolution", "2");

    Option::GetInstance().WriteFile();

    Texture* high = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/High.png");

	Entity* entity = MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("activeShadowQuality");
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetPos({ 0.35f, 0.80f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetSize({ (float)high->GetWidth() / 1920.0f, (float)high->GetHeight() / 1080.0f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetColor({ 0.30f, 0.20f, 0.25f });
}

void onVolumePlus(const std::string& name)
{
    if (std::stof(Option::GetInstance().GetVariable("f_masterVolume")) < 10)
    {
        std::ostringstream str;
        str << std::fixed << std::setprecision(1) << std::stof(Option::GetInstance().GetVariable("f_masterVolume")) + 0.1f;
        Option::GetInstance().SetVariable("f_masterVolume", str.str());

        Option::GetInstance().WriteFile();
        AudioEngine::GetInstance().ChangeMasterVolume(std::stof(str.str()));
    }
}

void onVolumeMinus(const std::string& name)
{
    if (std::stof(Option::GetInstance().GetVariable("f_masterVolume")) > 0)
    {
        std::ostringstream str;
        str << std::fixed << std::setprecision(1) << std::stof(Option::GetInstance().GetVariable("f_masterVolume")) - 0.1f;
        Option::GetInstance().SetVariable("f_masterVolume", str.str());

        Option::GetInstance().WriteFile();
        AudioEngine::GetInstance().ChangeMasterVolume(std::stof(str.str()));
    }
}

void onBox(const std::string& name)
{	
	bool active = std::atof(Option::GetInstance().GetVariable("i_music").c_str());
	Option::GetInstance().SetVariable("i_music", std::to_string(!active));
	Option::GetInstance().WriteFile();

    if (std::atof(Option::GetInstance().GetVariable("i_music").c_str()))
    {
        SceneManager::GetInstance().GetActiveScene()->GetEntity("player")->GetComponent<component::Audio2DVoiceComponent>()->Play(L"MenuMusic");
    }
    else
    {
        SceneManager::GetInstance().GetActiveScene()->GetEntity("player")->GetComponent<component::Audio2DVoiceComponent>()->Stop(L"MenuMusic");
    }
    
	float4 blended = { 1.0, 1.0, 1.0, 1.0 };
	if (active)
	{
		blended.w = 0.0;
	}

	Texture* emptyBox = AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/emptybox.png");

	Entity* entity = MainMenuHandler::GetInstance().GetOptionScene()->GetEntity("check");
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetPos({ 0.88f, 0.53f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetSize({ (float)emptyBox->GetWidth() / 1920.0f / 3.0f, (float)emptyBox->GetHeight() / 1080.0f / 3.0f });
	entity->GetComponent<component::GUI2DComponent>()->GetQuadManager()->SetBlend(blended);
}

void onMouseSensitivityPlus(const std::string& name)
{
    if (std::stof(Option::GetInstance().GetVariable("f_sensitivityX")) < 10)
    {
        std::ostringstream str;

        // X
        str << std::fixed << std::setprecision(1) << std::stof(Option::GetInstance().GetVariable("f_sensitivityX")) + 0.1f;
        Option::GetInstance().SetVariable("f_sensitivityX", str.str());

        // Y
        str.str("");
        str << std::fixed << std::setprecision(1) << std::stof(Option::GetInstance().GetVariable("f_sensitivityY")) + 0.1f;
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
        str << std::fixed << std::setprecision(1) << std::stof(Option::GetInstance().GetVariable("f_sensitivityX")) - 0.1f;
        Option::GetInstance().SetVariable("f_sensitivityX", str.str());

        // Y
        str.str("");
        str << std::fixed << std::setprecision(1) << std::stof(Option::GetInstance().GetVariable("f_sensitivityY")) - 0.1f;
        Option::GetInstance().SetVariable("f_sensitivityY", str.str());

        Option::GetInstance().WriteFile();
    }
}


void MainMenuUpdateScene(SceneManager* sm, double dt)
{
    Input::GetInstance().RegisterControllers();
    MenuUpdateScene(sm, dt);
}

void MenuUpdateScene(SceneManager* sm, double dt)
{
    Input::GetInstance().ReadControllerInput(dt);
}


static float totalScrollY = 0;

void CreditsUpdateScene(SceneManager* sm, double dt)
{
    const float scrollSpeed = -0.2; // -0.2
    float frameYChange = scrollSpeed * dt;
    totalScrollY += frameYChange;

    const float resetYOn = -8;
    if (totalScrollY < resetYOn)
    {
        ResetCreditsScene(sm);
        return;
    }

    Scene* creditsScene = sm->GetActiveScene();

    Entity* text = creditsScene->GetEntity("ScrollingText");
    component::GUI2DComponent* comp = text->GetComponent<component::GUI2DComponent>();

    auto map = comp->GetTextManager()->GetTextDataMap();
    auto it = map.begin();
    while (it != map.end())
    {
        std::string name = (*it).first;

		float2 oldPos = comp->GetTextManager()->GetText(name)->GetPos();

        comp->GetTextManager()->SetPos({ oldPos.x, oldPos.y + frameYChange }, name);

        it++;
    }
}

void ResetCreditsScene(SceneManager* sm)
{
    Scene* creditsScene = sm->GetScene("CreditsScene");

    Entity* text = creditsScene->GetEntity("ScrollingText");
    component::GUI2DComponent* comp = text->GetComponent<component::GUI2DComponent>();

    float frameYChange = -totalScrollY;

    auto map = comp->GetTextManager()->GetTextDataMap();
    auto it = map.begin();
    while (it != map.end())
    {
        std::string name = (*it).first;

        float2 oldPos = comp->GetTextManager()->GetText(name)->GetPos();
        
        comp->GetTextManager()->SetPos({ oldPos.x, oldPos.y + frameYChange }, name);

        it++;
    }

    totalScrollY = 0;
}