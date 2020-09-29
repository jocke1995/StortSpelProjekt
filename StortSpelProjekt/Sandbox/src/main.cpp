#include "Engine.h"
#include "Components/PlayerInputComponent.h"
#include "EnemyFactory.h"
#include "GameNetwork.h"

Scene* LeosTestScene(SceneManager* sm);
Scene* TimScene(SceneManager* sm);
Scene* JockesTestScene(SceneManager* sm);
Scene* FredriksTestScene(SceneManager* sm);
Scene* WilliamsTestScene(SceneManager* sm);
Scene* AndresTestScene(SceneManager* sm);
Scene* BjornsTestScene(SceneManager* sm);
Scene* AntonTestScene(SceneManager* sm);


void(*UpdateScene)(SceneManager*);
void LeoUpdateScene(SceneManager* sm);

void DefaultUpdateScene(SceneManager* sm);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /*------ Load Option Variables ------*/
    Option* option = &Option::GetInstance();
    option->ReadFile();
    float updateRate = 1.0f / std::atof(option->GetVariable("f_updateRate").c_str());

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

    sceneManager->SetScene(LeosTestScene(sceneManager));
    //sceneManager->SetScene(TimScene(sceneManager));
    //sceneManager->SetScene(JockesTestScene(sceneManager));
    //sceneManager->SetScene(FredriksTestScene(sceneManager));
    //sceneManager->SetScene(WilliamsTestScene(sceneManager));
    //sceneManager->SetScene(BjornsTestScene(sceneManager));
    //sceneManager->SetScene(AntonTestScene(sceneManager));
    //sceneManager->SetScene(AndresTestScene(sceneManager)); // example play and updateEmitter functions in AndresTestScene

    GameNetwork gameNetwork;

    /*------ Network Init -----*/
    bool networkOn = false;
    Network network;

    gameNetwork.SetNetwork(&network);

    if (std::atoi(option->GetVariable("i_network").c_str()) == 1)
    {
        gameNetwork.SetScene(sceneManager->GetScene("AntonScene"));
        gameNetwork.SetSceneManager(sceneManager);

        network.SetPlayerEntityPointer(sceneManager->GetScene("AntonScene")->GetEntity("player"), 0);
        network.ConnectToIP(option->GetVariable("s_ip"), std::atoi(option->GetVariable("i_port").c_str()));

        networkOn = true;
    }
    int networkCount = 0;
    double logicTimer = 0;
    int count = 0;

    while (!window->ExitWindow())
    {
        /* ------ Update ------ */
        timer->Update();
        logicTimer += timer->GetDeltaTime();

        renderer->RenderUpdate(timer->GetDeltaTime());
        if (logicTimer >= updateRate)
        {
            logicTimer = 0;
            networkCount++;
            physics->Update(updateRate);
            renderer->Update(updateRate);
        }

        /* ---- Network ---- */
        if (networkOn)
        {
            if (networkCount == 2) {
                networkCount = 0;

                network.SendPositionPacket();
                while(network.ListenPacket());
            }
        }

        /* ------ Sort ------ */
        renderer->SortObjects();
        
        UpdateScene(sceneManager);

        /* ------ Draw ------ */
        renderer->Execute();
    }
    return 0;
}

Scene* LeosTestScene(SceneManager* sm)
{
    // Create scene
    Scene* scene = sm->CreateScene("ThatSceneWithThemThereAiFeaturesAndStuff");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::InputComponent* ic = nullptr;
    component::Audio3DListenerComponent* lc = nullptr;
    component::Audio3DEmitterComponent* ec = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::AccelerationComponent* ac = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/playerBall.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");

    // Get the audio needed and add settings to it.
    AudioBuffer* loopedSound = al->LoadAudio(L"../Vendor/Resources/Audio/AGameWithNoName.wav", L"Music");

    loopedSound->SetAudioLoop(0);

    /* ---------------------- Player ---------------------- */
    Entity* entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    lc = entity->AddComponent<component::Audio3DListenerComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    //ac = entity->AddComponent<component::AccelerationComponent>(0.982);

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    ic->Init();
    bbc->Init();

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

    /* ---------------------- Stefan ---------------------- */
    entity = scene->AddEntity("stefan");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(posterModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 35, 1);
    tc->GetTransform()->SetPosition(0.0f, 17.5f, 35.0f);
    tc->GetTransform()->SetRotationX(3 * 3.1415 / 2);
    tc->GetTransform()->SetRotationZ(3 * 3.1415 / 2);

    /* ---------------------- Spotlight ---------------------- */
    entity = scene->AddEntity("Spotlight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW_MEDIUM_RESOLUTION);
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    ec = entity->AddComponent<component::Audio3DEmitterComponent>();
    ec->AddVoice(L"Music");

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(0.0f, 17.5f, 0.0f);

    slc->SetColor({ 50.0f, 0.0f, 50.0f });
    slc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    slc->SetDirection({ 0.0, 0.0, 1.0f });

    bbc->Init();

    /* ---------------------- dirLight ---------------------- */
    entity = scene->AddEntity("dirLight");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_HIGH_RESOLUTION);
    dlc->SetColor({ 0.0f, 0.5f, 0.5f });
    dlc->SetDirection({ -1.0f, -1.0f, 1.0f });

    /* ---------------------- Enemy -------------------------------- */
    EnemyFactory enH(scene);
    enH.AddEnemy("sphere", sphereModel, float3{ 0, 10, -5 }, F_COMP_FLAGS::OBB, 1.0, float3{ 1.578, 0, 0 });
    enH.AddExistingEnemy("sphere", float3{ 0, 10, -55 });
    enH.AddExistingEnemy("sphere", float3{ 25, 10, -30 });
    enH.AddExistingEnemy("sphere", float3{ -25, 10, -30 });

    /* ---------------------- Update Function ---------------------- */
    UpdateScene = &LeoUpdateScene;

    srand(time(NULL));

    return scene;
}

Scene* TimScene(SceneManager* sm)
{

    // Create Scene
    Scene* scene = sm->CreateScene("TimScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");

    // Get the audio needed and add settings to it.
    AudioBuffer* bruhSound = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav",L"Bruh");

    // Audio may loop infinetly (0) once (1) or otherwise specified amount of times!
    bruhSound->SetAudioLoop(0);

    /* ---------------------- Player ---------------------- */
    Entity* entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();


    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    avc->AddVoice(L"Bruh");
    avc->Play(L"Bruh");
    /* ---------------------- Player ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    /* ---------------------- PointLight1 ---------------------- */
    entity = scene->AddEntity("pointLight1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0, 4.0f, 15.0f);

    /* ---------------------- PointLight1 ---------------------- */

    return scene;
}

Scene* AntonTestScene(SceneManager* sm)
{
    // Create scene
    Scene* scene = sm->CreateScene("AntonScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::InputComponent* ic = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");

    /* ---------------------- Player 0 ---------------------- */
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    ic->Init();

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    /* ---------------------- PointLight ---------------------- */
    entity = scene->AddEntity("pointLight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(0.0f, 4.0f, 0.0f);

    plc->SetColor({ 2.0f, 0.0f, 2.0f });
    plc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    /* ---------------------- PointLight ---------------------- */

    return scene;
}

Scene* JockesTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("scene1");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::InputComponent* ic = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    /* ---------------------- Player ---------------------- */
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    /* ---------------------- Player ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    /* ---------------------- PointLight ---------------------- */
    entity = scene->AddEntity("pointLight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(0.0f, 4.0f, 0.0f);

    plc->SetColor({2.0f, 0.0f, 2.0f });
    plc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    /* ---------------------- PointLight ---------------------- */

    /* ---------------------- dirLight ---------------------- */
    entity = scene->AddEntity("dirLight");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_HIGH_RESOLUTION);
    dlc->SetColor({ 1.0f, 1.0f, 1.0f });
    dlc->SetDirection({ -1.0f, -1.0f, -1.0f });
    /* ---------------------- dirLight ---------------------- */

    /* ---------------------- Spotlight ---------------------- */
    entity = scene->AddEntity("Spotlight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW_MEDIUM_RESOLUTION);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, 10.0f);

    slc->SetColor({ 0.0f, 0.0f, 4.0f });
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
    slc->SetDirection({ -2.0, -1.0, 0.0f });
    /* ---------------------- Spotlight ---------------------- */

    return scene;
}

Scene* FredriksTestScene(SceneManager* sm)
{
	// Create Scene
	Scene* scene = sm->CreateScene("scene1");

	component::CameraComponent* cc = nullptr;
	component::ModelComponent* mc = nullptr;
	component::TransformComponent* tc = nullptr;
	component::PointLightComponent* plc = nullptr;
	AssetLoader* al = AssetLoader::Get();

	// Get the models needed
	Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
	Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
	Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
	Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");

	/* ---------------------- Player ---------------------- */
	Entity* entity = scene->AddEntity("player");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);

	mc->SetModel(playerModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc->GetTransform()->SetScale(1.0f);
	tc->GetTransform()->SetPosition(0, 1, -30);
	/* ---------------------- Player ---------------------- */

	/* ---------------------- Floor ---------------------- */
	entity = scene->AddEntity("floor");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();

	mc = entity->GetComponent<component::ModelComponent>();
	mc->SetModel(floorModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc = entity->GetComponent<component::TransformComponent>();
	tc->GetTransform()->SetScale(35, 1, 35);
	tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
	/* ---------------------- Floor ---------------------- */

	/* ---------------------- Stone ---------------------- */
	entity = scene->AddEntity("stone");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();

	mc = entity->GetComponent<component::ModelComponent>();
	mc->SetModel(stoneModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
	tc = entity->GetComponent<component::TransformComponent>();
	tc->GetTransform()->SetScale(0.01f);
	tc->GetTransform()->SetPosition(-8.0f, 0.0f, 0.0f);
	/* ---------------------- Stone ---------------------- */

	/* ---------------------- PointLight1 ---------------------- */
	entity = scene->AddEntity("pointLight1");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

	mc->SetModel(cubeModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
	tc->GetTransform()->SetScale(0.5f);
	tc->GetTransform()->SetPosition(-30.0f, 4.0f, 15.0f);

	/* ---------------------- PointLight1 ---------------------- */

	/* ---------------------- PointLigh2 ---------------------- */
	entity = scene->AddEntity("pointLight2");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

	mc->SetModel(cubeModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
	tc->GetTransform()->SetScale(0.5f);
	tc->GetTransform()->SetPosition(0.0f, 4.0f, 15.0f);

	plc->SetAttenuation({ 1.0f, 0.045f, 0.0075 });
	/* ---------------------- PointLight2 ---------------------- */

	/* ---------------------- PointLight3 ---------------------- */
	entity = scene->AddEntity("pointLight3");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

	mc->SetModel(cubeModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
	tc->GetTransform()->SetScale(0.5f);
	tc->GetTransform()->SetPosition(30.0f, 4.0f, 15.0f);

	/* ---------------------- PointLight3 ---------------------- */

	/* ---------------------- PointLight4 ---------------------- */
	entity = scene->AddEntity("pointLight4");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

	mc->SetModel(cubeModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
	tc->GetTransform()->SetScale(0.5f);
	tc->GetTransform()->SetPosition(-30.0f, 4.0f, -15.0f);

	/* ---------------------- PointLight4 ---------------------- */

	/* ---------------------- PointLigh5 ---------------------- */
	entity = scene->AddEntity("pointLight5");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

	mc->SetModel(cubeModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
	tc->GetTransform()->SetScale(0.5f);
	tc->GetTransform()->SetPosition(0.0f, 4.0f, -15.0f);

	plc->SetAttenuation({ 1.0f, 0.045f, 0.0075 });


	/* ---------------------- PointLight5 ---------------------- */

	/* ---------------------- PointLight6 ---------------------- */
	entity = scene->AddEntity("pointLight6");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();
	plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

	mc->SetModel(cubeModel);
	mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
	tc->GetTransform()->SetScale(0.5f);
	tc->GetTransform()->SetPosition(30.0f, 4.0f, -15.0f);

	/* ---------------------- PointLight6 ---------------------- */

	/* ------------------------- Text --------------------------- */

	// Load fonts
	std::pair<Font*, Texture*> arialFont = al->LoadFontFromFile(L"Arial.fnt");

	std::string textToRender = "Daedalus Maze 2:\nThe Return of the Minotaur";
	float2 textPos = { 0.02f, 0.01f };
	float2 textPadding = { 0.5f, 0.0f };
	float4 textColor = { 1.0f, 0.2f, 1.0f, 1.0f };
	float2 textScale = { 0.5f, 0.5f };

	scene->AddEntity("text");

	entity = scene->GetEntity("text");
	component::TextComponent* textComp = entity->AddComponent<component::TextComponent>(arialFont);
	textComp->AddText("text");
	textComp->SetColor(textColor, "text");
	textComp->SetPadding(textPadding, "text");
	textComp->SetPos(textPos, "text");
	textComp->SetScale(textScale, "text");
	textComp->SetText(textToRender, "text");

	/* ---------------------------------------------------------- */

	return scene;

}

Scene* WilliamsTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("scene1");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* dragonModel = al->LoadModel(L"../Vendor/Resources/Models/Dragon/Dragon 2.5_fbx.fbx");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* amongUsModel = al->LoadModel(L"../Vendor/Resources/Models/amongus/AmongUs.fbx");

    Entity* entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    component::PlayerInputComponent* ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    tc = entity->AddComponent<component::TransformComponent>();
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);


    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);


    entity = scene->AddEntity("dragon");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(dragonModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, -20.0f, 70.0f);
    tc->GetTransform()->SetRotationX(1.5708);


    entity = scene->AddEntity("amongUs");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(amongUsModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 5.0f, 40.0f);



    entity = scene->AddEntity("pointLight1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-30.0f, 4.0f, 15.0f);

    entity = scene->AddEntity("pointLight2");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0.0f, 4.0f, 15.0f);

    plc->SetAttenuation({ 1.0f, 0.045f, 0.0075 });


    entity = scene->AddEntity("pointLight3");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, 15.0f);

    entity = scene->AddEntity("pointLight4");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-30.0f, 4.0f, -15.0f);

    entity = scene->AddEntity("pointLight5");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0.0f, 4.0f, -15.0f);

    plc->SetAttenuation({ 1.0f, 0.045f, 0.0075 });

    entity = scene->AddEntity("pointLight6");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, -15.0f);

    return scene;
}

Scene* AndresTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("AndresTestScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::InputComponent* ic = nullptr;
    component::Audio3DListenerComponent* audioListener = nullptr;
    component::Audio3DEmitterComponent* audioEmitter = nullptr;
    component::Audio2DVoiceComponent* backgroundAudio = nullptr;

    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");

    // Get the audio needed and add settings to it.
    AudioBuffer* melodySound = al->LoadAudio(L"../Vendor/Resources/Audio/melody.wav", L"melody");
    AudioBuffer* bruhSound = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"bruh");
    AudioBuffer* horseSound = al->LoadAudio(L"../Vendor/Resources/Audio/AGameWithNoName.wav", L"horse");


    // Audio may loop infinetly (0) once (1) or otherwise specified amount of times!
    bruhSound->SetAudioLoop(0);
    melodySound->SetAudioLoop(0);
    horseSound->SetAudioLoop(0);

    /* ---------------------- Player ---------------------- */
    Entity* entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    audioListener = entity->AddComponent<component::Audio3DListenerComponent>();
    ic->Init();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, 0);

    /* ---------------------- Player ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    backgroundAudio = entity->AddComponent<component::Audio2DVoiceComponent>();
    backgroundAudio->AddVoice(L"bruh");
    backgroundAudio->Play(L"bruh");

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    /* ---------------------- PointLight1 ---------------------- */
    entity = scene->AddEntity("pointLight1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);
    audioEmitter = entity->AddComponent<component::Audio3DEmitterComponent>();
    audioEmitter->AddVoice(L"melody");
    //audioEmitter->Play(L"melody"); // example how to play the sound, commented away because without updateEmitter, they seem to be positioned very close to the ear (loud!)
    //audioEmitter->UpdateEmitter(L"melody");   // this needs to be in an update function to work properly, only here to show how to call it

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0, 4.0f, 15.0f);

    plc->SetColor({ 1.0f, 0.0f, 1.0f });
    /* ---------------------- PointLight1 ---------------------- */

    /* ---------------------- PointLight2 ---------------------- */
    entity = scene->AddEntity("pointLight2");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);
    audioEmitter = entity->AddComponent<component::Audio3DEmitterComponent>();
    audioEmitter->AddVoice(L"horse");
    //audioEmitter->Play(L"horse"); // example how to play the sound, commented away because without updateEmitter, they seem to be positioned very close to the ear (loud!)
    //audioEmitter->UpdateEmitter(L"horse");    // this needs to be in an update function to work properly, only here to show how to call it

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0, 4.0f, -15.0f);

    plc->SetColor({ 1.0f, 1.0f, 0.0f });
    /* ---------------------- PointLight2 ---------------------- */

    return scene;
}

Scene* BjornsTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("scene1");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* dragonModel = al->LoadModel(L"../Vendor/Resources/Models/Dragon/Dragon 2.5_fbx.fbx");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    // Get the audio needed and add settings to it.
    AudioBuffer* bruhSound = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");

    // Audio may loop infinetly (0) once (1) or otherwise specified amount of times!
    bruhSound->SetAudioLoop(1);

    Entity* entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    component::PlayerInputComponent* ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();
    // adding OBB with collision
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    avc->AddVoice(L"Bruh");

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    // initialize OBB after we have the transform info
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);


    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);


    entity = scene->AddEntity("dragon");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(dragonModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    bbc->Init();
    tc->GetTransform()->SetPosition(0.0f, -20.0f, 70.0f);
    tc->GetTransform()->SetRotationX(1.5708);
 
    Physics::GetInstance().AddCollisionEntity(entity);

    //entity = scene->AddEntity("stone");
    //mc = entity->AddComponent<component::ModelComponent>();
    //tc = entity->AddComponent<component::TransformComponent>();
    //bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    //mc = entity->GetComponent<component::ModelComponent>();
    //mc->SetModel(stoneModel);
    //mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    //tc = entity->GetComponent<component::TransformComponent>();
    //tc->GetTransform()->SetScale(0.01f);
    //tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    //bbc->Init();
    //Physics::GetInstance().AddCollisionEntity(entity);

    // Adding enemy example
    EnemyFactory enH(scene);
    enH.AddEnemy("rock", stoneModel, float3{ 1, 0, 1 }, F_COMP_FLAGS::OBB, 0.01, float3{ 1.578, 0, 0 });
    // showing that using the wrong overload will send Warning to Log. 
    // and then automaticly use the correct overloaded function 
   // enH.AddEnemy("rock", stoneModel, float3{ -10, 0, -10 }, F_COMP_FLAGS::OBB, 0.01);

    // adding an already existing enemy type but changing the scale of it
    enH.AddExistingEnemyWithChanges("rock", float3{ 20, 0, 4 }, UINT_MAX, 0.005f);

    // looping through and adding already existing enemy type with only new position
    float xVal = 8;
    float zVal = 0;
    for (int i = 0; i < 50; i++)
    {
        zVal += 8;
        enH.AddExistingEnemy("rock", float3{ xVal, 0, zVal });
        if ((i + 1) % 5 == 0)
        {
            xVal += 8;
            zVal = 0;
        }
    }


    /* ---------------------- PointLight ---------------------- */
    entity = scene->AddEntity("pointLight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(0.0f, 4.0f, 0.0f);

    plc->SetColor({ 2.0f, 0.0f, 2.0f });
    plc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    /* ---------------------- PointLight ---------------------- */

    /* ---------------------- dirLight ---------------------- */
    entity = scene->AddEntity("dirLight");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_HIGH_RESOLUTION);
    dlc->SetColor({ 1.0f, 1.0f, 1.0f });
    dlc->SetDirection({ -1.0f, -1.0f, -1.0f });
    /* ---------------------- dirLight ---------------------- */

    /* ---------------------- Spotlight ---------------------- */
    entity = scene->AddEntity("Spotlight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW_MEDIUM_RESOLUTION);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, 10.0f);

    slc->SetColor({ 0.0f, 0.0f, 4.0f });
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
    slc->SetDirection({ -2.0, -1.0, 0.0f });
    /* ---------------------- Spotlight ---------------------- */

    return scene;
}

void LeoUpdateScene(SceneManager* sm)
{
    component::Audio3DEmitterComponent* ec = sm->GetScene("ThatSceneWithThemThereAiFeaturesAndStuff")->GetEntity("Spotlight")->GetComponent<component::Audio3DEmitterComponent>();
    ec->UpdateEmitter(L"Music");
    ec->Play(L"Music");
}

void DefaultUpdateScene(SceneManager* sm)
{
}
