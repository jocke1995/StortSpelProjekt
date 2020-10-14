#include "Engine.h"
#include "Components/MeleeComponent.h"
#include "Components/PlayerInputComponent.h"
#include "Components/HealthComponent.h"
#include "Components/RangeComponent.h"
#include "EnemyFactory.h"
#include "GameNetwork.h"

Scene* JacobsTestScene(SceneManager* sm);
Scene* LeosTestScene(SceneManager* sm);
Scene* LeosBounceScene(SceneManager* sm);
Scene* TimScene(SceneManager* sm);
Scene* JockesTestScene(SceneManager* sm);
Scene* FloppipTestScene(SceneManager* sm);
Scene* FredriksTestScene(SceneManager* sm);
Scene* WilliamsTestScene(SceneManager* sm);
Scene* AndresTestScene(SceneManager* sm);
Scene* BjornsTestScene(SceneManager* sm);
Scene* AntonTestScene(SceneManager* sm);


void(*UpdateScene)(SceneManager*);
void LeoUpdateScene(SceneManager* sm);
void LeoBounceUpdateScene(SceneManager* sm);
void TimUpdateScene(SceneManager* sm);
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

    //Scene* jacobScene = JacobsTestScene(sceneManager);
    Scene* leoScene = LeosTestScene(sceneManager);
    //Scene* leoBounceScene = LeosBounceScene(sceneManager);
    //Scene* timScene = TimScene(sceneManager);
    //Scene* jockeScene = JockesTestScene(sceneManager);
    //Scene* filipScene = FloppipTestScene(sceneManager);
    //Scene* fredrikScene = FredriksTestScene(sceneManager);
    //Scene* williamScene = WilliamsTestScene(sceneManager);
    //Scene* bjornScene = BjornsTestScene(sceneManager);
    //Scene* antonScene = AntonTestScene(sceneManager);
    //Scene* andresScene = AndresTestScene(sceneManager);

    Scene* activeScenes[] = { leoScene };

    // Set scene
    sceneManager->SetScenes(1, activeScenes);

    GameNetwork gameNetwork;

    /*------ Network Init -----*/
    Network network;

    gameNetwork.SetNetwork(&network);

    if (std::atoi(option->GetVariable("i_network").c_str()) == 1)
    {
        gameNetwork.SetScenes(sceneManager->GetActiveScenes());
        gameNetwork.SetSceneManager(sceneManager);
    }
    double networkTimer = 0;
    double logicTimer = 0;
    int count = 0;

    static int nr = 0;
    static Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    while (!window->ExitWindow())
    {
        /* ------ Update ------ */
        UpdateScene(sceneManager);

        timer->Update();
        logicTimer += timer->GetDeltaTime();
        if (network.IsConnected())
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
            if (networkTimer >= networkUpdateRate) {
                networkTimer = 0;

                network.SendPositionPacket();
                while (network.ListenPacket());
            }
        }

        if (window->WasSpacePressed())
        {
            Entity* a = new Entity(std::to_string(nr++));
            auto b = a->AddComponent<component::ModelComponent>();
            b->SetModel(sphereModel);

            auto tc = a->AddComponent<component::TransformComponent>();
            tc->GetTransform()->SetPosition(leoScene->GetEntity("player")->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionXMFLOAT3());

            sceneManager->AddEntity(a, leoScene);
        }
        else if (window->WasTabPressed())
        {
            sceneManager->RemoveEntity(leoScene->GetEntity(std::to_string(--nr)), leoScene);
        }

        /* ------ Sort ------ */
        renderer->SortObjects();

        /* ------ Draw ------ */
        renderer->Execute();
    }
    sceneManager->ResetScene();
    return 0;
}

Scene* JacobsTestScene(SceneManager* sm)
{
    Scene* scene = sm->CreateScene("BounceScene");

    AssetLoader* al = AssetLoader::Get();

    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    /*--------------------- Assets ---------------------*/

    /*--------------------- Component declarations ---------------------*/
    Entity* entity = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PlayerInputComponent* pic = nullptr;
    component::TextComponent* txc = nullptr;
    component::CollisionComponent* bcc = nullptr;
    component::MeleeComponent* melc = nullptr;

    /*--------------------- Player ---------------------*/
    // entity
    entity = scene->AddEntity("player");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    pic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    bbc = entity->AddComponent < component::BoundingBoxComponent>();
    melc = entity->AddComponent<component::MeleeComponent>();

    Transform* t = tc->GetTransform();

    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(-15.0f, 10.0f, 0.0f);

    bcc = entity->AddComponent<component::CubeCollisionComponent>(1.0f, 1.0f, 1.0f, 1.0f, 0.01f, 0.0f);
    pic->Init();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);

    avc->AddVoice(L"Bruh");

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

    /*--------------------- Sphere2 ---------------------*/
    // entity
    entity = scene->AddEntity("Sphere2");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(-5.0f, 1.0f, 3.5f);
    bcc = entity->AddComponent<component::SphereCollisionComponent>(5.0f, 1.5f, 0.0f, 1.0f);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);

    /*--------------------- Box ---------------------*/
    // entity
    entity = scene->AddEntity("Box");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(5.0f, 1.0f, 4.0f);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(1000.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

    mc->SetModel(cubeModel);

    /*--------------------- Floor ---------------------*/
    // entity
    entity = scene->AddEntity("floor");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 35.0, 0.0, 35.0);


    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(35.0f, 1.0f, 35.0f);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

    /* ---------------------- PointLight1 ---------------------- */
    entity = scene->AddEntity("pointLight1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0, 4.0f, 15.0f);

    /*--------------------- DirectionalLight ---------------------*/
    // entity
    entity = scene->AddEntity("sun");

    // components
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetDirection({ 1.0f, -1.0f, -1.0f });
    dlc->SetColor({ 0.5f, 0.5f, 0.5f });

    /* ---------------------- Update Function ---------------------- */
    UpdateScene = &LeoBounceUpdateScene;

    return scene;
}

Scene* LeosTestScene(SceneManager* sm)
{
    // Create scene
    Scene* scene = sm->CreateScene("ThatSceneWithThemThereDashFeaturesAndStuff");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::InputComponent* ic = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::CollisionComponent* ccc = nullptr;
    component::HealthComponent* hc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Man/man.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");
    Model* barbModel = al->LoadModel(L"../Vendor/Resources/Models/Barb/conan_obj.obj");
    HeightmapModel* heightMapModel = al->LoadHeightmap(L"../Vendor/Resources/Textures/HeightMaps/tst.hm");

    double3 entityDim;

#pragma region entities

#pragma region player
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0.0, 20.0, -200.0);

    double3 playerDim = mc->GetModelDim();

    double rad = playerDim.z / 2.0;
    double cylHeight = playerDim.y - (rad * 2.0) ;
    ccc = entity->AddComponent<component::CapsuleCollisionComponent>(1.0, rad, cylHeight, 0.0, 0.0, false);
    hc = entity->AddComponent<component::HealthComponent>(50);
    ic->Init();
#pragma endregion

#pragma region floor
    // heightmap
    HeightMapInfo inf;
    inf.data = heightMapModel->GetHeights();
    inf.length = heightMapModel->GetLength();
    inf.width = heightMapModel->GetWidth();
    inf.maxHeight = 1;
    inf.minHeight = -1;

    // entity
    entity = scene->AddEntity("floor");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ccc = entity->AddComponent<component::HeightmapCollisionComponent>(inf);

    mc->SetModel(heightMapModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(1.0f, 10.0f, 1.0f);
    tc->GetTransform()->SetPosition(0.0f, -1.0f, 0.0f);
    tc->GetTransform()->SetRotationY(PI / 2);
#pragma endregion

#pragma region walls
    entity = scene->AddEntity("wall0");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(10, 1, 250);
    tc->GetTransform()->SetRotationX(3 * 3.1415 / 2);
    tc->GetTransform()->SetRotationZ(3 * 3.1415 / 2);
    tc->GetTransform()->SetPosition(0.0f, 5.0f, 249.0f);
    ccc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 250.0, 0.0, 1.0);


    entity = scene->AddEntity("wall1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(10, 1, 250);
    tc->GetTransform()->SetRotationX(5 * 3.1415 / 2);
    tc->GetTransform()->SetRotationZ(3 * 3.1415 / 2);
    tc->GetTransform()->SetPosition(0.0f, 5.0f, -249.0f);
    ccc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 250.0, 0.0, 1.0);


    entity = scene->AddEntity("wall2");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(10, 1, 250);
    tc->GetTransform()->SetRotationX(2 * 3.1415);
    tc->GetTransform()->SetRotationZ(3 * 3.1415 / 2);
    tc->GetTransform()->SetPosition(-249.0f, 5.0f, 0.0f);
    ccc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 250.0, 0.0, 1.0);


    entity = scene->AddEntity("wall3");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(10, 1, 250);
    tc->GetTransform()->SetRotationX(3.1415);
    tc->GetTransform()->SetRotationZ(3 * 3.1415 / 2);
    tc->GetTransform()->SetPosition(249.0f, 5.0f, 0.0f);
    ccc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 250.0, 0.0, 1.0);
#pragma endregion

#pragma region cube
    entity = scene->AddEntity("Cube");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ccc = entity->AddComponent<component::CubeCollisionComponent>(1.0, 1.0, 1.0, 1.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 10.0f, 15.0f);
    tc->GetTransform()->SetRotationX(PI / 4);
    tc->GetTransform()->SetRotationY(PI / 4);
    tc->GetTransform()->SetRotationZ(PI / 4);
#pragma endregion

#pragma region ball
    entity = scene->AddEntity("Ball1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetPosition(-16.0f, 10.0f, -133.0f);

    entityDim = mc->GetModelDim();

    ccc = entity->AddComponent<component::SphereCollisionComponent>(1.0, entityDim.y / 2.0, 1.0, 0.0);
#pragma endregion 1

#pragma region ball
    entity = scene->AddEntity("Ball2");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetPosition(0.0f, 10.0f, 0.0f);

    entityDim = mc->GetModelDim();

    ccc = entity->AddComponent<component::SphereCollisionComponent>(1.0, entityDim.y / 2.0, 1.0, 0.0);
#pragma endregion 2

#pragma region stefan
    entity = scene->AddEntity("stefan");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(posterModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetRotationX(-3 * 3.1415 / 4);
    tc->GetTransform()->SetRotationZ(3 * 3.1415 / 2);
    tc->GetTransform()->SetPosition(70.0f, 50.0f, 70.0f);
    ccc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0, 0.0, 1.0);
#pragma endregion

#pragma region hasse
    entity = scene->AddEntity("hasse");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc->SetModel(posterModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetRotationX(3 * 3.1415 / 4);
    tc->GetTransform()->SetRotationZ(3 * 3.1415 / 2);
    tc->GetTransform()->SetPosition(-70.0f, 50.0f, 70.0f);
    ccc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0, 0.0, 1.0);
#pragma endregion

#pragma region enemies
    EnemyFactory enH(scene);
    /*enH.AddEnemy("sphere", sphereModel, 10, float3{ -50, 10, 50 },L"Bruh", L"attack", F_COMP_FLAGS::OBB | F_COMP_FLAGS::SPHERE_COLLISION, F_AI_FLAGS::CAN_ROLL, 10.0, float3{ 1.578, 0, 0 });
    enH.AddExistingEnemy("sphere", float3{ 50, 10, -50 });
    enH.AddExistingEnemy("sphere", float3{ 50, 10, 50 });
    enH.AddExistingEnemy("sphere", float3{ -50, 10, -50 });
    enH.AddExistingEnemyWithChanges("sphere", float3{ -1, 15, -31 }, F_COMP_FLAGS::OBB | F_COMP_FLAGS::SPHERE_COLLISION, F_AI_FLAGS::CAN_JUMP | F_AI_FLAGS::CAN_ROLL, 0.5);*/
    enH.AddEnemy("conan", barbModel, 20, float3{ 245.0, 10.0, 245.0 }, L"Bruh", L"attack", F_COMP_FLAGS::OBB | F_COMP_FLAGS::CAPSULE_COLLISION, 0, 0.3, float3{ 0.0, 0.0, 0.0 }, "Ball2", 500.0f, 0.0f);
    enH.AddExistingEnemy("conan", float3{ 245, 10, -245 });
    enH.AddExistingEnemy("conan", float3{ -245, 10, 245 });
    enH.AddExistingEnemy("conan", float3{ -245, 10, -245 });
#pragma endregion

#pragma endregion

#pragma region lights

#pragma region pointlight
    entity = scene->AddEntity("pointLight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(0.0f, 10.0f, 15.0f);

    plc->SetColor({ 5.0f, 0.0f, 5.0f });
    plc->SetAttenuation({ 1.0, 0.09f, 0.032f });
#pragma endregion

#pragma region spotlight
    entity = scene->AddEntity("Spotlight1");
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    tc->GetTransform()->SetPosition(-250.0, 50.0, -250.0);

    slc->SetColor({ 1.0f, 1.0f, 1.0f });
    slc->SetAttenuation({ 0.000005f, 0.000005f, 0.000005f });
    slc->SetDirection({ 250.0, -50.0, 250.0f });
#pragma endregion 1

#pragma region spotlight
    entity = scene->AddEntity("Spotlight2");
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    tc->GetTransform()->SetPosition(250.0, 50.0, -250.0);

    slc->SetColor({ 1.0f, 1.0f, 1.0f });
    slc->SetAttenuation({ 0.000005f, 0.000005f, 0.000005f });
    slc->SetDirection({ -250.0, -50.0, 250.0f });
#pragma endregion 2

#pragma region spotlight
    entity = scene->AddEntity("Spotlight3");
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    tc->GetTransform()->SetPosition(-250.0, 50.0, 250.0);

    slc->SetColor({ 1.0f, 1.0f, 1.0f });
    slc->SetAttenuation({ 0.000005f, 0.000005f, 0.000005f });
    slc->SetDirection({ 250.0, -50.0, -250.0f });
#pragma endregion 3

#pragma region spotlight
    entity = scene->AddEntity("Spotlight4");
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    tc->GetTransform()->SetPosition(250.0, 50.0, 250.0);

    slc->SetColor({ 1.0f, 1.0f, 1.0f });
    slc->SetAttenuation({ 0.000005f, 0.000005f, 0.000005f });
    slc->SetDirection({ -250.0, -50.0, -250.0f });
#pragma endregion 4

#pragma region spotlight
    entity = scene->AddEntity("SpotlightHasse");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(-35.0f, 35.0f, 35.0f);

    slc->SetColor({ 50.0f, 50.0f, 0.0f });
    slc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    slc->SetDirection({ -35.0, 15.0, 35.0f });

    bbc->Init();
#pragma endregion Hasse

#pragma region spotlight
    entity = scene->AddEntity("SpotlightStefan");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(35.0f, 35.0f, 35.0f);

    slc->SetColor({ 0.0f, 50.0f, 50.0f });
    slc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    slc->SetDirection({ 35.0, 15.0, 35.0f });

    bbc->Init();
#pragma endregion Stefan

#pragma region dirlight
    entity = scene->AddEntity("dirLight");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetColor({ 0.5f, 0.5f, 0.5f });
    dlc->SetDirection({ -1.0f, -1.0f, 1.0f });
#pragma endregion

#pragma endregion

#pragma region skybox
    TextureCubeMap* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    sbc->SetTexture(skyboxCubemap);
#pragma endregion

#pragma region navmesh
    NavMesh navmesh;
    float3 pos;
    float2 size;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    size.x = 0.5;
    size.y = 0.5;
    NavQuad* nav1 = navmesh.AddNavQuad(pos, size);

    pos.x = 1;
    pos.y = 0;
    pos.z = 0;
    size.x = 0.5;
    size.y = 0.5;
    NavQuad* nav2 = navmesh.AddNavQuad(pos, size);
    pos.x = 0.5;
    pos.y = 0;
    navmesh.ConnectNavQuads(nav1, nav2, pos);

    pos.x = 1;
    pos.y = 1.5;
    pos.z = 0;
    size.x = 1;
    size.y = 1;
    NavQuad* nav3 = navmesh.AddNavQuad(pos, size);
    pos.x = 0.0;
    pos.y = 0.5;
    navmesh.ConnectNavQuads(nav3, nav1, pos);
    pos.x = 1.0;
    pos.y = 0.5;
    navmesh.ConnectNavQuads(nav3, nav2, pos);

    pos.x = 1.1;
    pos.y = 0.3;
    nav1 = navmesh.GetQuad(pos);
    nav1 = nav1->connections.at(0)->GetConnectedQuad(nav1);
#pragma endregion

    /* ---------------------- Update Function ---------------------- */
    UpdateScene = &LeoUpdateScene;

    srand(time(NULL));

    return scene;
}

Scene* LeosBounceScene(SceneManager* sm)
{
    Scene* scene = sm->CreateScene("BounceScene");

    AssetLoader* al = AssetLoader::Get();

    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Gandalf/gandalf.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    /*--------------------- Assets ---------------------*/

    /*--------------------- Component declarations ---------------------*/
    Entity* entity = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PlayerInputComponent* pic = nullptr;
    component::TextComponent* txc = nullptr;
    component::CollisionComponent* bcc = nullptr;

    /*--------------------- Player ---------------------*/
    // entity
    entity = scene->AddEntity("player");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    pic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();

    Transform* t = tc->GetTransform();

    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(-15.0f, 10.0f, 0.0f);

    bcc = entity->AddComponent<component::CapsuleCollisionComponent>(1.0f, 1.3f, 3.2f, 0.01f, 0.0f);
    pic->Init();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);

    avc->AddVoice(L"Bruh");

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
    
    /*--------------------- Sphere2 ---------------------*/
    // entity
    entity = scene->AddEntity("Sphere2");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(-5.0f, 1.0f, 3.5f);
    bcc = entity->AddComponent<component::SphereCollisionComponent>(5.0f, 1.5f, 0.0f, 1.0f);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);

    /*--------------------- Box ---------------------*/
    // entity
    entity = scene->AddEntity("Box");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(5.0f, 1.0f, 4.0f);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(1000.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f);

    mc->SetModel(cubeModel);

    /*--------------------- Floor ---------------------*/
    // entity
    entity = scene->AddEntity("floor");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 350.0, 0.0, 350.0);


    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(350.0f, 1.0f, 350.0f);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

    /* ---------------------- PointLight1 ---------------------- */
    entity = scene->AddEntity("pointLight1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(0, 4.0f, 15.0f);

    /*--------------------- DirectionalLight ---------------------*/
    // entity
    entity = scene->AddEntity("sun");

    // components
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetDirection({ 1.0f, -1.0f, -1.0f });
    dlc->SetColor({ 0.5f, 0.5f, 0.5f });

    /* ---------------------- Update Function ---------------------- */
    UpdateScene = &LeoBounceUpdateScene;

    return scene;
}

Scene* TimScene(SceneManager* sm)
{
    Scene* scene = sm->CreateScene("TimScene");

    AssetLoader* al = AssetLoader::Get();

    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    HeightmapModel* heightMapModel = al->LoadHeightmap(L"../Vendor/Resources/Textures/HeightMaps/hm.hm");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");

    /*--------------------- Assets ---------------------*/

    /*--------------------- Component declarations ---------------------*/
    Entity* entity = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PlayerInputComponent* pic = nullptr;
    component::TextComponent* txc = nullptr;
    component::CollisionComponent* ccc = nullptr;
    component::SphereCollisionComponent* scc = nullptr;
    /*--------------------- Component declarations ---------------------*/

    /*--------------------- Player ---------------------*/
    // entity
    entity = scene->AddEntity("player");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    pic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();

    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0.0f, 5.0f, 0.0f);

    ccc = entity->AddComponent<component::CubeCollisionComponent>(1,1,1,1,0,0);
    pic->Init();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);

    avc->AddVoice(L"Bruh");
    /*--------------------- Player ---------------------*/

   // /*--------------------- Box1 ---------------------*/
   // // entity
   // entity = scene->AddEntity("Box1");
   //
   // // components
   // mc = entity->AddComponent<component::ModelComponent>();
   // tc = entity->AddComponent<component::TransformComponent>();
   //
   // tc->GetTransform()->SetScale(1.0f);
   // tc->GetTransform()->SetPosition(1.0f, 1.0f, 1.0f);
   // //scc = entity->AddComponent<component::SphereCollisionComponent>();
   //
   // mc->SetModel(cubeModel);
   // mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
   //
   // /*--------------------- Box1 ---------------------*/
   //
   // /*--------------------- Box2 ---------------------*/
   // // entity
   // entity = scene->AddEntity("Box2");
   //
   // // components
   // mc = entity->AddComponent<component::ModelComponent>();
   // mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
   // tc = entity->AddComponent<component::TransformComponent>();
   // tc->GetTransform()->SetScale(1.0f);
   // tc->GetTransform()->SetPosition(1.0f, 1.0f, 10.0f);
   // //ccc = entity->AddComponent<component::CubeCollisionComponent>(1000.0);
   //
   // mc->SetModel(cubeModel);
   //
   // /*--------------------- Box2 ---------------------*/


    for (int i = 0; i < 25; i++)
    {
        entity = scene->AddEntity("sphere" + std::to_string(i));

        mc = entity->AddComponent<component::ModelComponent>();
        tc = entity->AddComponent<component::TransformComponent>();
        ccc = entity->AddComponent<component::SphereCollisionComponent>(1,0.5);

        mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
        mc->SetModel(sphereModel);
        tc->GetTransform()->SetPosition(i * 10 % 50 - 25.0f, 10, i * 10/ 5.0f - 25.0f);
        tc->GetTransform()->SetScale(1.5f / 6.0f);
    }


    /*--------------------- Floor ---------------------*/
    // entity
    entity = scene->AddEntity("floor");

    // components
    //mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    //ccc = entity->AddComponent<component::CubeCollisionComponent>(0.0,35.0,0.0,35.0);

    
    HeightMapInfo inf;
    inf.data = heightMapModel->GetHeights();
    inf.length = 50.0;
    inf.width = 50.0;
    inf.maxHeight = 1;
    inf.minHeight = 0;

    ccc = entity->AddComponent<component::HeightmapCollisionComponent>(inf);

    //mc->SetModel(heightMapModel);
    //mc->SetModel(floorModel);
    //mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(1.0f, 10.f, 1.0f);
    tc->GetTransform()->SetPosition(0.0f, -1.0f, 0.0f);
    /*--------------------- Floor ---------------------*/

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

    /*--------------------- DirectionalLight ---------------------*/
    // entity
    entity = scene->AddEntity("sun");

    // components
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetDirection({ 1.0f, -1.0f, -1.0f });
    dlc->SetColor({ 0.5f, 0.5f, 0.5f });
    /*--------------------- DirectionalLight ---------------------*/

    UpdateScene = &TimUpdateScene;

    return scene;
}

Scene* JockesTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("jockesScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::InputComponent* ic = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::CollisionComponent* bcc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");

    /* ---------------------- Player ---------------------- */
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    bcc = entity->AddComponent<component::SphereCollisionComponent>(1, 1.5, 0.0);
    scene->SetPrimaryCamera(cc->GetCamera());
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
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 35.0, 0.0, 35.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

     /* ---------------------- Cube ---------------------- */
    entity = scene->AddEntity("Cube");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(1.0, 1.0, 1.0, 1.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 1.0f, -25.0f);
    tc->GetTransform()->SetRotationX(PI / 4);
    tc->GetTransform()->SetRotationY(PI / 4);
    tc->GetTransform()->SetRotationZ(PI / 4);

    //bcc->Rotate({ 0.0f, 1.0f, 1.0f }, PI / 4);
    /* ---------------------- Cube ---------------------- */

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
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetColor({ 1.0f, 1.0f, 1.0f });
    dlc->SetDirection({ -1.0f, -1.0f, -1.0f });
    /* ---------------------- dirLight ---------------------- */

    /* ---------------------- Spotlight ---------------------- */
    entity = scene->AddEntity("Spotlight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(30.0f, 4.0f, 10.0f);

    slc->SetColor({ 0.0f, 0.0f, 8.0f });
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
    slc->SetDirection({ -2.0, -1.0, 0.0f });
    /* ---------------------- Spotlight ---------------------- */

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
    component::InputComponent* ic = nullptr;
    component::CollisionComponent* bcc = nullptr;

    Entity* entity = nullptr;

    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    /* ---------------------- Player ---------------------- */
    entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::SphereCollisionComponent>(1, 1.5, 0.0);
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    /* ---------------------- Player ---------------------- */

    /* ---------------------- Skybox ---------------------- */

    // Skybox
    TextureCubeMap* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    sbc->SetTexture(skyboxCubemap);

    /* ---------------------- Skybox ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 35.0, 0.0, 35.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    
    /* ---------------------- PointLight1 ---------------------- */
    entity = scene->AddEntity("pointLight1");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    plc = entity->AddComponent<component::PointLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION);

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-30.0f, 4.0f, 15.0f);

    plc->SetColor({ 2.0f, 0.0f, 2.0f });
    plc->SetAttenuation({ 1.0, 0.09f, 0.032f });

    component::BoundingBoxComponent* bbc = entity->AddComponent<component::BoundingBoxComponent>();
    bbc->Init();

    /* ---------------------- PointLight1 ---------------------- */

    /* ---------------------- The Sun ---------------------- */
    entity = scene->AddEntity("sun");
    component::DirectionalLightComponent* dlc;
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    
    dlc->SetColor({ 2.0f, 2.0f, 2.0f });

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
    Scene* scene = sm->CreateScene("williamScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::CollisionComponent* bcc = nullptr;

    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* dragonModel = al->LoadModel(L"../Vendor/Resources/Models/Dragon/Dragon 2.5_fbx.fbx");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* aniTest = al->LoadModel(L"../Vendor/Resources/Models/aniTest/Standard_Walk.fbx");
    Model* amongUsModel = al->LoadModel(L"../Vendor/Resources/Models/amongus/AmongUs.fbx");

    Entity* entity = scene->AddEntity("player");
    bcc = entity->AddComponent<component::CubeCollisionComponent>(1, 1, 1, 1, 0.1);
    mc = entity->AddComponent<component::ModelComponent>();
    component::PlayerInputComponent* ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    tc = entity->AddComponent<component::TransformComponent>();
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);

    /* ---------------------- Skybox ---------------------- */

    // Skybox
    TextureCubeMap* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    sbc->SetTexture(skyboxCubemap);

    /* ---------------------- Skybox ---------------------- */

    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 35.0, 0.0, 35.0);

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
    mc->SetModel(aniTest);
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
    //Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
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
    //avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    //avc->AddVoice(L"Bruh");

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
    dlc->SetColor({ 0.3f, 0.3f, 0.3f });
    dlc->SetDirection({ -0.01f, -1.0f, 0.01f });
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
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
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
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
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
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
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
    slc->SetAttenuation({ 1.0f, 0.027f, 0.0028f });
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

    /*entity = enH.AddExistingEnemy("enemy", float3{ 40, 10, 0 });
    entity = enH.AddExistingEnemy("enemy", float3{ 0, 10, -40 });
    entity = enH.AddExistingEnemy("enemy", float3{ -40, 10, 0 });*/
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

Scene* BjornsTestScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("bjornScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::HealthComponent* hc = nullptr;
    component::RangeComponent* rc = nullptr;
    component::CollisionComponent* bcc = nullptr;
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
    bcc = entity->AddComponent<component::CubeCollisionComponent>(1, 1, 1, 1, 0.1);
    component::PlayerInputComponent* ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();
    hc = entity->AddComponent<component::HealthComponent>(15);
    // adding OBB with collision
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    avc->AddVoice(L"Bruh");
    rc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3);

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    // initialize OBB after we have the transform info
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);


    entity = scene->AddEntity("floor");
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 35.0, 0.0, 35.0);
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
    //enH.AddEnemy("rock", stoneModel, 5, float3{ 1, 0, 1 }, F_COMP_FLAGS::OBB, 0.01, float3{ 1.578, 0, 0 });
    // showing that using the wrong overload will send Warning to Log. 
    // and then automaticly use the correct overloaded function 
   // enH.AddEnemy("rock", stoneModel, float3{ -10, 0, -10 }, F_COMP_FLAGS::OBB, 0.01);

    // adding an already existing enemy type but changing the scale of it
    //enH.AddExistingEnemyWithChanges("rock", float3{ 20, 0, 4 }, UINT_MAX, 0.005f);

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
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetColor({ 1.0f, 1.0f, 1.0f });
    dlc->SetDirection({ -1.0f, -1.0f, -1.0f });
    /* ---------------------- dirLight ---------------------- */

    /* ---------------------- Spotlight ---------------------- */
    entity = scene->AddEntity("Spotlight");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::USE_TRANSFORM_POSITION | FLAG_LIGHT::CAST_SHADOW);

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
}

void LeoBounceUpdateScene(SceneManager* sm)
{
    // Check if the command "RESET" has been executed
    if (ImGuiHandler::GetInstance().GetBool("reset") == true)
    {
        ImGuiHandler::GetInstance().SetBool("reset", false);

        component::CollisionComponent* cc = sm->GetScene("BounceScene")->GetEntity("player")->GetComponent<component::CollisionComponent>();
        cc->SetPosition(-15.0f, 10.0f, 0.0f);
        cc->SetVelVector(0.0f, 0.0f, 0.0f);
        cc->SetRotation(0.0f, 0.0f, 0.0f);
        cc->SetAngularVelocity(0.0f, 0.0f, 0.0f);

        cc = sm->GetScene("BounceScene")->GetEntity("Sphere1")->GetComponent<component::CollisionComponent>();
        cc->SetPosition(1.0f, 1.0f, 1.0f);
        cc->SetVelVector(0.0f, 0.0f, 0.0f);
        cc->SetRotation(0.0f, 0.0f, 0.0f);
        cc->SetAngularVelocity(0.0f, 0.0f, 0.0f);

        cc = sm->GetScene("BounceScene")->GetEntity("Sphere2")->GetComponent<component::CollisionComponent>();
        cc->SetPosition(-5.0f, 1.0f, 3.5f);
        cc->SetVelVector(0.0f, 0.0f, 0.0f);
        cc->SetRotation(0.0f, 0.0f, 0.0f);
        cc->SetAngularVelocity(0.0f, 0.0f, 0.0f);

        cc = sm->GetScene("BounceScene")->GetEntity("Box")->GetComponent<component::CollisionComponent>();
        cc->SetPosition(5.0f, 1.0f, 4.0f);
        cc->SetVelVector(0.0f, 0.0f, 0.0f);
        cc->SetRotation(0.0f, 0.0f, 0.0f);
        cc->SetAngularVelocity(0.0f, 0.0f, 0.0f);
    }
    else
    {
        component::CollisionComponent* cc = sm->GetScene("BounceScene")->GetEntity("Box")->GetComponent<component::CollisionComponent>();
        cc->SetAngularVelocity(0.0f, 10.0f, 0.0f);
    }
}

void TimUpdateScene(SceneManager* sm)
{
    if (ImGuiHandler::GetInstance().GetBool("reset"))
    {
        ImGuiHandler::GetInstance().SetBool("reset", false);

        sm->GetScene("TimScene")->GetEntity("player")->GetComponent<component::CollisionComponent>()->SetVelVector(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("player")->GetComponent<component::CollisionComponent>()->SetAngularVelocity(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("player")->GetComponent<component::CollisionComponent>()->SetRotation(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("player")->GetComponent<component::CollisionComponent>()->SetPosition(0, 10, 0);

        sm->GetScene("TimScene")->GetEntity("Box1")->GetComponent<component::CollisionComponent>()->SetVelVector(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("Box1")->GetComponent<component::CollisionComponent>()->SetAngularVelocity(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("Box1")->GetComponent<component::CollisionComponent>()->SetRotation(0, 0, 0);
        sm->GetScene("TimScene")->GetEntity("Box1")->GetComponent<component::CollisionComponent>()->SetPosition(1, 1, 1);

        sm->GetScene("TimScene")->GetEntity("Box2")->GetComponent<component::CollisionComponent>()->SetAngularVelocity(0, 3.14, 0);
        sm->GetScene("TimScene")->GetEntity("Box2")->GetComponent<component::CollisionComponent>()->SetFriction(0);

    }
}

void DefaultUpdateScene(SceneManager* sm)
{
}

void DemoUpdateScene(SceneManager* sm)
{
    component::Audio3DEmitterComponent* ec = sm->GetScene("AndresTestScene")->GetEntity("enemy")->GetComponent<component::Audio3DEmitterComponent>();
    ec->UpdateEmitter(L"Bruh");

    std::string name = "enemy";
    for (int i = 1; i < 76; i++)
    {
        name = "enemy" + std::to_string(i);
        ec = sm->GetScene("AndresTestScene")->GetEntity(name)->GetComponent<component::Audio3DEmitterComponent>();
        ec->UpdateEmitter(L"Bruh");
    }
}