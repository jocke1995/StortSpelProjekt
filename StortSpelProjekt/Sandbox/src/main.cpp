#include "Engine.h"
#include "Components/PlayerInputComponent.h"
#include "EnemyFactory.h"

Scene* LeosTestScene(SceneManager* sm);
Scene* TimScene(SceneManager* sm);
Scene* JockesTestScene(SceneManager* sm);
Scene* FredriksTestScene(SceneManager* sm);
Scene* WilliamsTestScene(SceneManager* sm);
Scene* AndresTestScene(SceneManager* sm);
Scene* BjornsTestScene(SceneManager* sm); // showing example of AddEnemy() from enemy handler


void(*UpdateScene)(SceneManager*);
void LeoUpdateScene(SceneManager* sm);

void DefaultUpdateScene(SceneManager* sm);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	/*------ Load Option Variables ------*/
	Option::GetInstance().ReadFile();
	float updateRate = 1.0f / Option::GetInstance().GetVariable("updateRate");

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

    //sceneManager->SetSceneToDraw(LeosTestScene(sceneManager));
    //sceneManager->SetSceneToDraw(TimScene(sceneManager));
    //sceneManager->SetSceneToDraw(JockesTestScene(sceneManager));
    //sceneManager->SetSceneToDraw(FredriksTestScene(sceneManager));
    //sceneManager->SetSceneToDraw(WilliamsTestScene(sceneManager));
    //sceneManager->SetSceneToDraw(BjornsTestScene(sceneManager));
    sceneManager->SetSceneToDraw(AndresTestScene(sceneManager));
    // AndresTestScene is testing 3d audio sound. The Audio3DEmitterComponents (horse and melody) and Audio3DListenerComponent are automatically updateded through calls to renderer->update for testing purposes.
    // Feel free to try and break it, I have tested so that same sound can be used for multiple entities, different sounds for different entities and same sound as 3d and background at same time.

    /*----- Timer ------*/
    double logicTimer = 0;

    while (!window->ExitWindow())
    {
        /* ------ Update ------ */
        timer->Update();
        logicTimer += timer->GetDeltaTime();

        renderer->RenderUpdate(timer->GetDeltaTime());
        if (logicTimer >= updateRate)
        {
            logicTimer = 0;
            physics->Update(updateRate);
            renderer->Update(updateRate);
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
    Scene* scene = sm->CreateScene("ThatSceneWithThemThereImGuiFeaturesAndStuff");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::AudioVoiceComponent* avc = nullptr;
    component::InputComponent* ic = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");

    // Get the audio needed and add settings to it.
    AudioBuffer* loopedSound = al->LoadAudio(L"../Vendor/Resources/Audio/AGameWithNoName.wav", L"Music");

    loopedSound->SetAudioLoop(0);

    /* ---------------------- Player ---------------------- */
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::AudioVoiceComponent>();
    component::BoundingBoxComponent* bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    avc->AddVoice(L"Music");
    avc->Play(L"Music");
    ic->Init();
    bbc->Init();

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

    /* ---------------------- PointLight1 ---------------------- */
    entity = scene->AddEntity("pointLight1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0, 8.0f, -15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.5f, 0.0f, 0.5f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 5.0f, 5.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.9f, 0.9f, 1.0f });

    // Set variiables for ImGui
    ImGuiHandler::GetInstance().SetFloat("LightPositionZ", -15.0f);
    ImGuiHandler::GetInstance().SetFloat4("LightColor", float4({ 1.0f, 1.0f, 1.0f, 1.0f }));

    // Set UpdateScene function
    UpdateScene = &LeoUpdateScene;

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
    component::AudioVoiceComponent* avc = nullptr;
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
    avc = entity->AddComponent<component::AudioVoiceComponent>();


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

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.0f, 0.5f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 10.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.9f, 0.0f, 1.0f });
    /* ---------------------- PointLight1 ---------------------- */

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
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");

    /* ---------------------- Player ---------------------- */
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();

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
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::PICKING);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(stoneModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.01f);
    tc->GetTransform()->SetPosition(-8.0f, 0.0f, 0.0f);
    bbc->Init();
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

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.05f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 10.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.9f, 0.0f, 1.0f });

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

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.00f, 0.05f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 0.00f, 10.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.00f, 0.9f, 1.0f });

    plc->SetAttenuation({1.0f, 0.045f, 0.0075});
    

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

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.05f, 0.05f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 10.0f, 10.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.9f, 0.9f, 1.0f });

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

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 30.0f, 0.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.0f, 0.0f, 1.0f });

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

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.05f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 15.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.9f, 0.0f, 1.0f });

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

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.0f, 0.05f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 0.0f, 30.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.0f, 0.9f, 1.0f });

    /* ---------------------- PointLight6 ---------------------- */


    /* ---------------------- The Sun ---------------------- */
    entity = scene->AddEntity("sun");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION);
    
    dlc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.0f, 0.0f, 1.0f });
    dlc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 1.0f, 0.1f, 0.1f, 1.0f });
    dlc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 1.0f, 0.1f, 0.1f, 1.0f });
    dlc->SetDirection({ -1.0f, -1.0f, -1.0f });
    /* ---------------------- The Sun ---------------------- */

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

	plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.05f, 0.0f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 10.0f, 0.0f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.9f, 0.0f, 1.0f });
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

	plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.00f, 0.05f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 0.00f, 10.0f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.00f, 0.9f, 1.0f });

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

	plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.05f, 0.05f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 10.0f, 10.0f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.9f, 0.9f, 1.0f });
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

	plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.0f, 0.0f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 30.0f, 0.0f, 0.0f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.0f, 0.0f, 1.0f });

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

	plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.05f, 0.0f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 15.0f, 0.0f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.9f, 0.0f, 1.0f });

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

	plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.0f, 0.05f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 0.0f, 30.0f, 1.0f });
	plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.0f, 0.9f, 1.0f });

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

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.05f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 10.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.9f, 0.0f, 1.0f });


    entity = scene->AddEntity("pointLight2");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0.0f, 4.0f, 15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.00f, 0.05f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 0.00f, 10.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.00f, 0.9f, 1.0f });

    plc->SetAttenuation({ 1.0f, 0.045f, 0.0075 });


    entity = scene->AddEntity("pointLight3");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, 15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.05f, 0.05f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 10.0f, 10.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.9f, 0.9f, 1.0f });


    entity = scene->AddEntity("pointLight4");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-30.0f, 4.0f, -15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 30.0f, 0.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.0f, 0.0f, 1.0f });


    entity = scene->AddEntity("pointLight5");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0.0f, 4.0f, -15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.05f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 15.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.9f, 0.0f, 1.0f });

    plc->SetAttenuation({ 1.0f, 0.045f, 0.0075 });


    entity = scene->AddEntity("pointLight6");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, -15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.0f, 0.05f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 0.0f, 30.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.0f, 0.9f, 1.0f });


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
    component::AudioVoiceComponent* backgroundAudio = nullptr;

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
    backgroundAudio = entity->AddComponent<component::AudioVoiceComponent>();
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
    //audioEmitter->AddVoice(L"horse");
    audioEmitter->Play(L"melody");
    //audioEmitter->Play(L"horse");

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0, 4.0f, 15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.0f, 0.5f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 10.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.9f, 0.0f, 1.0f });
    /* ---------------------- PointLight1 ---------------------- */

    /* ---------------------- PointLight2 ---------------------- */
    entity = scene->AddEntity("pointLight2");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);
    audioEmitter = entity->AddComponent<component::Audio3DEmitterComponent>();
    //audioEmitter->AddVoice(L"melody");
    audioEmitter->AddVoice(L"horse");
    //audioEmitter->Play(L"melody");
    audioEmitter->Play(L"horse");

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0, 4.0f, -15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.0f, 0.5f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 10.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.9f, 0.0f, 1.0f });
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
    component::BoundingBoxComponent* bbc = nullptr;
    component::AudioVoiceComponent* avc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* dragonModel = al->LoadModel(L"../Vendor/Resources/Models/Dragon/Dragon 2.5_fbx.fbx");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");

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
    avc = entity->AddComponent<component::AudioVoiceComponent>();
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
    float yVal = 0;
    for (int i = 0; i < 50; i++)
    {
        yVal += 8;
        enH.AddExistingEnemy("rock", float3{ xVal, 0, yVal });
        if ((i + 1) % 5 == 0)
        {
            xVal += 8;
            yVal = 0;
        }
    }


    //entity = scene->AddEntity("pointLight2");
    //mc = entity->AddComponent<component::ModelComponent>();
    //tc = entity->AddComponent<component::TransformComponent>();
    //plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    //mc->SetModel(cubeModel);
    //mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    //tc->GetTransform()->SetScale(0.5f);
    //tc->GetTransform()->SetPosition(0.0f, 4.0f, 15.0f);

    //plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.00f, 0.05f, 1.0f });
    //plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 0.00f, 10.0f, 1.0f });
    //plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.00f, 0.9f, 1.0f });

    //plc->SetAttenuation({ 1.0f, 0.045f, 0.0075 });


    //entity = scene->AddEntity("pointLight3");
    //mc = entity->AddComponent<component::ModelComponent>();
    //tc = entity->AddComponent<component::TransformComponent>();
    //plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    //mc->SetModel(cubeModel);
    //mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    //tc->GetTransform()->SetScale(0.5f);
    //tc->GetTransform()->SetPosition(30.0f, 4.0f, 15.0f);

    //plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.05f, 0.05f, 1.0f });
    //plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 10.0f, 10.0f, 1.0f });
    //plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.9f, 0.9f, 1.0f });


 /*   entity = scene->AddEntity("pointLight4");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-30.0f, 4.0f, -15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 30.0f, 0.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.0f, 0.0f, 1.0f });*/


   entity = scene->AddEntity("pointLight5");
   mc = entity->AddComponent<component::ModelComponent>();
   tc = entity->AddComponent<component::TransformComponent>();
   plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

   mc->SetModel(cubeModel);
   mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
   tc->GetTransform()->SetScale(0.5f);
   tc->GetTransform()->SetPosition(0.0f, 4.0f, -15.0f);

   plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.05f, 0.0f, 1.0f });
   plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 8.0f, 5.0f, 5.0f, 1.0f });
   plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.9f, 0.0f, 1.0f });

   plc->SetAttenuation({ 1.0f, 0.045f, 0.0075 });


    // Directional lighting
    entity = scene->AddEntity("sun");

    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION);
    dlc->SetDirection({ 1.0f, -1.0f, -1.0f });
    dlc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.1f, 0.1f, 0.1f, 1.0f });
    dlc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.5f, 0.5f, 0.5f, 1.0f });
    dlc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.5f, 0.5f, 0.5f, 1.0f });

    return scene;
}

void LeoUpdateScene(SceneManager* sm)
{

    if (DEVELOPERMODE_DEVINTERFACE == true)
    {
        float lightPos = ImGuiHandler::GetInstance().GetFloat("LightPositionZ");
        float4 lightColor = ImGuiHandler::GetInstance().GetFloat4("LightColor");

        sm->GetScene("ThatSceneWithThemThereImGuiFeaturesAndStuff")->GetEntity("pointLight1")->GetComponent<component::PointLightComponent>()->SetColor(COLOR_TYPE::LIGHT_AMBIENT, lightColor);
        sm->GetScene("ThatSceneWithThemThereImGuiFeaturesAndStuff")->GetEntity("pointLight1")->GetComponent<component::PointLightComponent>()->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { lightColor.x * 16.0f, lightColor.y * 16.0f, lightColor.z * 16.0f, lightColor.w });
        sm->GetScene("ThatSceneWithThemThereImGuiFeaturesAndStuff")->GetEntity("pointLight1")->GetComponent<component::PointLightComponent>()->SetColor(COLOR_TYPE::LIGHT_SPECULAR, lightColor);
        sm->GetScene("ThatSceneWithThemThereImGuiFeaturesAndStuff")->GetEntity("pointLight1")->GetComponent<component::TransformComponent>()->GetTransform()->SetPosition(DirectX::XMFLOAT3(0.0f, 8.0f, lightPos));
    }
}

void DefaultUpdateScene(SceneManager* sm)
{
}
