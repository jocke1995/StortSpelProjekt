#include "Engine.h"
#include "Components/PlayerInputComponent.h"
#include "Components/HealthComponent.h"
#include "Components/RangeComponent.h"
#include "Components/MeleeComponent.h"
#include "EnemyFactory.h"
#include "GameNetwork.h"

Scene* GetDemoScene(SceneManager* sm);
void(*UpdateScene)(SceneManager*);
void DemoUpdateScene(SceneManager* sm);
void DefaultUpdateScene(SceneManager* sm);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /*------ Load Option Variables ------*/
    Option* option = &Option::GetInstance();
    option->ReadFile();
    float updateRate = 1.0f / std::atof(option->GetVariable("f_updateRate").c_str());
    float networkUpdateRate = 1.0f / std::atof(option->GetVariable("f_networkUpdateRate").c_str());

    /* ------ Engine  ------ */
    Engine engine;
    engine.Init(hInstance, nCmdShow);

    /*  ------ Get references from engine  ------ */
    Window* const window = engine.GetWindow();
    Timer* const timer = engine.GetTimer();
    ThreadPool* const threadPool = engine.GetThreadPool();
    SceneManager* const sceneManager = engine.GetSceneHandler();
    Renderer* const renderer = engine.GetRenderer();
    Physics* const physics = engine.GetPhysics();
    AudioEngine* const audioEngine = engine.GetAudioEngine();


    /*------ AssetLoader to load models / textures ------*/
    AssetLoader* al = AssetLoader::Get();

    UpdateScene = &DefaultUpdateScene;

    /*----- Set the scene -----*/
    Scene* demoScene = GetDemoScene(sceneManager);
    sceneManager->SetScenes(1, &demoScene);

    GameNetwork gameNetwork;

    /*------ Network Init -----*/
    bool networkOn = false;
    Network network;

    gameNetwork.SetNetwork(&network);

    if (std::atoi(option->GetVariable("i_network").c_str()) == 1)
    {
        gameNetwork.SetScenes(sceneManager->GetActiveScenes());
        gameNetwork.SetSceneManager(sceneManager);

        networkOn = true;
    }
    double networkTimer = 0;
    double logicTimer = 0;
    int count = 0;


    while (!window->ExitWindow())
    {
        /* ------ Update ------ */
        UpdateScene(sceneManager);

        timer->Update();
        logicTimer += timer->GetDeltaTime();
        if (networkOn)
        {
            networkTimer += timer->GetDeltaTime();
        }

        sceneManager->RenderUpdate(timer->GetDeltaTime());
        if (logicTimer >= updateRate)
        {
            logicTimer = 0;
            sceneManager->Update(updateRate);
            physics->Update(updateRate);
        }

        /* ---- Network ---- */
        if (network.IsConnected())
        {
            if (networkTimer >= networkUpdateRate)
            {
                networkTimer = 0;

                network.SendPositionPacket();
                while (network.ListenPacket());
            }
        }

        /* ------ Sort ------ */
        renderer->SortObjects();

        /* ------ Draw ------ */
        renderer->Execute();
    }
    return 0;
}

Scene* GetDemoScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("DemoScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::InputComponent* ic = nullptr;
    component::Audio3DListenerComponent* audioListener = nullptr;
    component::Audio3DEmitterComponent* audioEmitter = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::HealthComponent* hc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CollisionComponent* bcc = nullptr;
    component::RangeComponent* rc = nullptr;

    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* enemyModel = al->LoadModel(L"../Vendor/Resources/Models/Barb/conan_obj.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");

    // Get the audio needed and add settings to it.
    AudioBuffer* melodySound = al->LoadAudio(L"../Vendor/Resources/Audio/melody.wav", L"melody");
    AudioBuffer* bruhSound = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    AudioBuffer* horseSound = al->LoadAudio(L"../Vendor/Resources/Audio/AGameWithNoName.wav", L"horse");
    AudioBuffer* attackSound = al->LoadAudio(L"../Vendor/Resources/Audio/attack.wav", L"attack");

    // Audio may loop infinetly (0) once (1) or otherwise specified amount of times!
    bruhSound->SetAudioLoop(0);
    melodySound->SetAudioLoop(0);
    horseSound->SetAudioLoop(0);
    attackSound->SetAudioLoop(1);

    /* ---------------------- Player ---------------------- */
    Entity* entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(1, 1, 1, 1, 0.01);
    audioListener = entity->AddComponent<component::Audio3DListenerComponent>();
    ic->Init();
    hc = entity->AddComponent<component::HealthComponent>(15);
    rc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3, 1, 10);
    // adding OBB with collision
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -40);
    // initialize OBB after we have the transform info
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);

    /* ---------------------- Player ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 60.0, 0.0, 60.0);
    //backgroundAudio = entity->AddComponent<component::Audio2DVoiceComponent>();
    //backgroundAudio->AddVoice(L"bruh");
    //backgroundAudio->Play(L"bruh");

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(60, 1, 60);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    /* ---------------------- dirLight ---------------------- */
    entity = scene->AddEntity("dirLight");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetColor({ 1.0f, 1.0f, 1.0f });
    dlc->SetDirection({ -1.0f, -1.0f, -1.0f });
    /* ---------------------- dirLight ---------------------- */

    /* ---------------------- Spotlights ---------------------- */
    entity = scene->AddEntity("SpotlightRed");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);
    
    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(-50.0f, 3.0f, 50.0f);
    
    slc->SetColor({ 10.0f, 0.0f, 0.0f });
    slc->SetAttenuation({ 1.0f, 0.14, 0.07f});
    slc->SetDirection({ 1.0, -0.5, -1.0f });
    
    entity = scene->AddEntity("SpotlightGreen");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);
    
    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(50.0f, 3.0f, 50.0f);
    
    slc->SetColor({ 0.0f, 10.0f, 0.0f });
    slc->SetAttenuation({ 1.0f, 0.14, 0.07f });
    slc->SetDirection({ -1.0, -0.5, -1.0f });
    
    entity = scene->AddEntity("SpotlightBlue");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);
    
    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(50.0f, 3.0f, -50.0f);
    
    slc->SetColor({ 0.0f, 0.0f, 10.0f });
    slc->SetAttenuation({ 1.0f, 0.14, 0.07f });
    slc->SetDirection({ -1.0, -0.5, 1.0f });

    entity = scene->AddEntity("SpotlightYellow");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(-50.0f, 3.0f, -50.0f);

    slc->SetColor({ 10.0f, 10.0f, 0.0f });
    slc->SetAttenuation({ 1.0f, 0.14, 0.07f });
    slc->SetDirection({ 1.0, -0.5, 1.0f });
    /* ---------------------- Spotlights ---------------------- */

    /*--------------------- Sphere1 ---------------------*/
    // entity
    entity = scene->AddEntity("Sphere1");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(1.0f, 1.0f, 1.0f);
    bcc = entity->AddComponent<component::SphereCollisionComponent>(1.0f, 1.5f, 1.0f, 1.0f);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    /*--------------------- Sphere1 ---------------------*/

    /* ---------------------- Stefan ---------------------- */
    entity = scene->AddEntity("stefan");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(posterModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(50, 50, 1);
    tc->GetTransform()->SetPosition(0.0f, 30.0f, 500.0f);
    tc->GetTransform()->SetRotationX(3 * 3.1415 / 2);
    tc->GetTransform()->SetRotationZ(3 * 3.1415 / 2);
    /* ---------------------- Stefan ---------------------- */


    /* ---------------------- Enemy -------------------------------- */
    EnemyFactory enH(scene);
    entity = enH.AddEnemy("enemy", enemyModel, 10, float3{ 0, 10, 40 }, L"Bruh", L"attack", F_COMP_FLAGS::OBB, 0, 0.3, float3{ 0, 0, 0 });

    // add bunch of enemies
    float xVal = 8;
    float zVal = 20;
    // extra 75 enemies, make sure to change number in for loop in DemoUpdateScene function if you change here
    for (int i = 0; i < 75; i++)
    {
        zVal += 8;
        entity = enH.AddExistingEnemy("enemy", float3{ xVal - 64, 1, zVal });
        if ((i + 1) % 5 == 0)
        {
            xVal += 8;
            zVal = 10;
        }
    }
    /* ---------------------- Enemy -------------------------------- */

	/* ------------------------- Text --------------------------- */
	std::string textToRender = "Daedalus Maze 2:\nThe Return of the Minotaur";
	float2 textPos = { 0.02f, 0.85f };
	float2 textPadding = { 0.5f, 0.0f };
	float4 textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float2 textScale = { 3.0f, 3.0f };

	scene->AddEntity("text");

	entity = scene->GetEntity("text");
	component::GUI2DComponent* textComp = entity->AddComponent<component::GUI2DComponent>();
	textComp->AddText("health");
	textComp->SetColor(textColor, "health");
	textComp->SetPadding(textPadding, "health");
	textComp->SetPos(textPos, "health");
	textComp->SetScale(textScale, "health");
	textComp->SetText(textToRender, "health");

	float2 quadPos = { 0.25f, 0.25f };
	float2 quadScale = { 0.5f, 0.5f };
	textComp->CreateQuad(quadPos, quadScale, true, L"../Vendor/Resources/Textures/2DGUI/replay.png");

	/* ---------------------------------------------------------- */

    /* ---------------------- Skybox ---------------------- */
    TextureCubeMap* skyboxCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    /* ---------------------- Skybox ---------------------- */


    /* ---------------------- Update Function ---------------------- */
    UpdateScene = &DemoUpdateScene;
    srand(time(NULL));
    /* ---------------------- Update Function ---------------------- */

    return scene;
}

void DefaultUpdateScene(SceneManager* sm)
{
}

void DemoUpdateScene(SceneManager* sm)
{
    component::Audio3DEmitterComponent* ec = sm->GetScene("DemoScene")->GetEntity("enemy")->GetComponent<component::Audio3DEmitterComponent>();
    ec->UpdateEmitter(L"Bruh");

	component::HealthComponent* hc = sm->GetScene("DemoScene")->GetEntity("player")->GetComponent<component::HealthComponent>();
	component::GUI2DComponent* tc = sm->GetScene("DemoScene")->GetEntity("text")->GetComponent<component::GUI2DComponent>();
	AssetLoader* al = AssetLoader::Get();
	Font* javaneseFont = al->LoadFontFromFile(L"Javanese.fnt");
	tc->SetText("HP: " + std::to_string(hc->GetHealth()), "health");
	tc->SetFont(javaneseFont);
	static float red = 0, green = 1, blue = 0.5;
	float4 color = float4{ abs(sin(red)), abs(sin(green)), abs(sin(blue)), 1.0};
	tc->SetColor(color, "health");
	red += 0.01;
	green += 0.01;
	blue += 0.01;
	tc->UploadTextData("health");

	float2 quadPos = { 0.25f, 0.25f };
	float2 quadScale = { 0.5f, 0.5f };
	tc->CreateQuad(quadPos, quadScale, true);

	if (tc->HasBeenPressed())
	{
		Log::Print("PRESSED!\n");
	}

    std::string name = "enemy";
    for (int i = 1; i < 76; i++)
    {
        name = "enemy" + std::to_string(i);
        ec = sm->GetScene("DemoScene")->GetEntity(name)->GetComponent<component::Audio3DEmitterComponent>();
        ec->UpdateEmitter(L"Bruh");
    }
}