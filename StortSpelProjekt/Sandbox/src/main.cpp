#include "Engine.h"
#include "GameEntity.h"

Scene* LeosTestScene(SceneManager* sm);
Scene* TimScene(SceneManager* sm);
Scene* JockesTestScene(SceneManager* sm);
Scene* FloppipTestScene(SceneManager* sm);
Scene* FredriksTestScene(SceneManager* sm);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /* ------ Engine  ------ */
    Engine engine = Engine();
    engine.Init(hInstance, nCmdShow);

    /*  ------ Get references from engine  ------ */
    Window* const window = engine.GetWindow();
    Timer* const timer = engine.GetTimer();
    ThreadPool* const threadPool = engine.GetThreadPool();
    SceneManager* const sceneManager = engine.GetSceneHandler();
    Renderer* const renderer = engine.GetRenderer();
    Physics* const physics = engine.GetPhysics();

    /*------ Load Option Variables ------*/
    Option::GetInstance().ReadFile();

    /*------ AssetLoader to load models / textures ------*/
    AssetLoader* al = AssetLoader::Get();

    //sceneManager->SetSceneToDraw(LeosTestScene(sceneManager));
    //sceneManager->SetSceneToDraw(TimScene(sceneManager));
    //sceneManager->SetSceneToDraw(JockesTestScene(sceneManager));
    sceneManager->SetSceneToDraw(FloppipTestScene(sceneManager));
    //sceneManager->SetSceneToDraw(FredriksTestScene(sceneManager));

    while (!window->ExitWindow())
    {
        /* ------ Update ------ */
        timer->Update();
        renderer->Update(timer->GetDeltaTime());
        physics->Update(timer->GetDeltaTime());

        /* ------ Sort ------ */
        renderer->SortObjectsByDistance();

        /* ------ Draw ------ */
        renderer->Execute();
    }
    return 0;
}

Scene* LeosTestScene(SceneManager* sm)
{
    // Create scene
    Scene* scene = sm->CreateScene("ThatSceneWithThemThereCameraFeaturesAndStuff");

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
    Entity* entity = static_cast<GameEntity*>(scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    tc = entity->AddComponent<component::TransformComponent>();
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::AudioVoiceComponent>();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    avc->AddVoice(L"Music");
    avc->Play(L"Music");
    ic->Init();

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0, 8.0f, -15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.5f, 0.0f, 0.5f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 5.0f, 5.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.9f, 0.9f, 1.0f });
    /* ---------------------- PointLight1 ---------------------- */

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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    /* ---------------------- Player ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, -15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.0f, 0.05f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 0.0f, 30.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.0f, 0.9f, 1.0f });

    /* ---------------------- PointLight6 ---------------------- */





    return scene;

}

Scene* FloppipTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("floppipScene");

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

    /* ---------------------- Player ---------------------- */
    Entity* entity = static_cast<GameEntity*>(scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    tc = entity->AddComponent<component::TransformComponent>();
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::AudioVoiceComponent>();
    ic->Init();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    /* ---------------------- Player ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, -15.0f);

    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.0f, 0.05f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 0.0f, 30.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.0f, 0.0f, 0.9f, 1.0f });

    /* ---------------------- PointLight6 ---------------------- */





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
	mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
	tc->GetTransform()->SetScale(1.0f);
	tc->GetTransform()->SetPosition(0, 1, -30);
	/* ---------------------- Player ---------------------- */

	/* ---------------------- Floor ---------------------- */
	entity = scene->AddEntity("floor");
	mc = entity->AddComponent<component::ModelComponent>();
	tc = entity->AddComponent<component::TransformComponent>();

	mc = entity->GetComponent<component::ModelComponent>();
	mc->SetModel(floorModel);
	mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
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
	mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
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
	mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
	mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
	mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
	mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
	mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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
	mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);
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

