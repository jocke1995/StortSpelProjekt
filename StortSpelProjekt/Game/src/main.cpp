#include "Engine.h"
#include "EnemyFactory.h"
#include "GameNetwork.h"

// Game includes
#include "Player.h"
#include "Shop.h"

Scene* GetDemoScene(SceneManager* sm);
Scene* ShopScene(SceneManager* sm);

void(*UpdateScene)(SceneManager*, double dt);

void DemoUpdateScene(SceneManager* sm, double dt);
void DefaultUpdateScene(SceneManager* sm, double dt);

void ShopUpdateScene(SceneManager* sm, double dt);

EnemyFactory enemyFactory;

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
    //Scene* shopScene = ShopScene(sceneManager);
    sceneManager->SetScenes(1, &demoScene);

    GameNetwork gameNetwork;

    /*------ Network Init -----*/

    if (std::atoi(option->GetVariable("i_network").c_str()) == 1)
    {
        gameNetwork.SetScenes(sceneManager->GetActiveScenes());
        gameNetwork.SetSceneManager(sceneManager);
        gameNetwork.SetEnemies(enemyFactory.GetAllEnemies());
    }
    double networkTimer = 0;
    double logicTimer = 0;
    int count = 0;


    while (!window->ExitWindow())
    {
        /* ------ Update ------ */
        timer->Update();

        UpdateScene(sceneManager, timer->GetDeltaTime());
        logicTimer += timer->GetDeltaTime();
        if (gameNetwork.IsConnected())
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
        if (gameNetwork.IsConnected())
        {
            if (networkTimer >= networkUpdateRate) {
                networkTimer = 0;

                gameNetwork.Update(networkUpdateRate);
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
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Man/man.obj");
    Model* enemyModel = al->LoadModel(L"../Vendor/Resources/Models/Zombie/zombie.obj");
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
    audioListener = entity->AddComponent<component::Audio3DListenerComponent>();
    ic->Init();
    hc = entity->AddComponent<component::HealthComponent>(15);
    rc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3, 1, 10);
    // adding OBB with collision
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    bcc = entity->AddComponent<component::CapsuleCollisionComponent>(100, mc->GetModelDim().z * 0.5, mc->GetModelDim().y - (mc->GetModelDim().z * 0.5), 0.0, 0.0, false);
    tc->GetTransform()->SetScale(0.5f);
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
    enemyFactory.SetScene(scene);
    enemyFactory.AddSpawnPoint({  0, 10, 40 });
    enemyFactory.AddSpawnPoint({ 10, 10, 0 });
    enemyFactory.AddSpawnPoint({ 20, 10, 10 });
    enemyFactory.DefineEnemy("Enemy", enemyModel, 10, L"Bruh", F_COMP_FLAGS::OBB | F_COMP_FLAGS::CAPSULE_COLLISION, 0, 0.04);

    // extra 75 enemies, make sure to change number in for loop in DemoUpdateScene function if you change here
    for (int i = 0; i < 75; i++)
    {
        entity = enemyFactory.SpawnEnemy("Enemy");
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

Scene* ShopScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("shopScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::InputComponent* ic = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::CollisionComponent* bcc = nullptr;
    component::MeleeComponent* mac = nullptr;
    component::RangeComponent* rc = nullptr;
    component::UpgradeComponent* uc = nullptr;
    component::CapsuleCollisionComponent* ccc = nullptr;
    component::HealthComponent* hc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Man/man.obj");
    Model* shopModel = al->LoadModel(L"../Vendor/Resources/Models/Shop/shop.obj");
    Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");
    Model* fenceModel = al->LoadModel(L"../Vendor/Resources/Models/FencePBR/fence.obj");

    TextureCubeMap* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");

#pragma region player
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    mac = entity->AddComponent<component::MeleeComponent>();
    rc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3, 1, 20);
    uc = entity->AddComponent<component::UpgradeComponent>();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0.0, 20.0, 0.0);

    double3 playerDim = mc->GetModelDim();

    double rad = playerDim.z / 2.0;
    double cylHeight = playerDim.y - (rad * 2.0);
    ccc = entity->AddComponent<component::CapsuleCollisionComponent>(200.0, rad, cylHeight, 0.0, 0.0, false);
    hc = entity->AddComponent<component::HealthComponent>(10000000);
    ic->Init();
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);

    Player::GetInstance().SetPlayer(entity);
    Player::GetInstance().GetShop()->RandomizeInventory();
#pragma endregion player
    /* ---------------------- Skybox ---------------------- */
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    sbc->SetTexture(skyboxCubemap);
    /* ---------------------- Skybox ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0, 0.0, 1.0);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(50, 1, 50);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

    /* ---------------------- Poster ---------------------- */
    entity = scene->AddEntity("poster");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(posterModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(1, 1, 1);
    tc->GetTransform()->SetRotationZ(-PI / 2);
    tc->GetTransform()->SetPosition(27.8f, 1.0f, 34.0f);
    /* ---------------------- Poster ---------------------- */

    /* ---------------------- Shop ---------------------- */
    entity = scene->AddEntity("shop");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(shopModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);

    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(30.0f, 0.0f, 30.0f);
    tc->GetTransform()->SetRotationY(PI + PI / 4);

    double3 shopDim = mc->GetModelDim();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(10000000.0, shopDim.x / 2.0f, shopDim.y / 2.0f, shopDim.z / 2.0f, 1000.0, 0.0, false);
    /* ---------------------- Shop ---------------------- */
#pragma region walls
    // Left wall
    entity = scene->AddEntity("wallLeft");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(fenceModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE | FLAG_DRAW::NO_DEPTH | FLAG_DRAW::GIVE_SHADOW);


    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(-50.0f, 10.0f, 0.0f);
    tc->GetTransform()->SetScale(10, 1, 50);
    tc->GetTransform()->SetRotationZ(-PI / 2);
    tc->GetTransform()->SetRotationY(PI);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0f, 0.0f, 1.0f);

    // Right wall
    entity = scene->AddEntity("wallRight");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(fenceModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE | FLAG_DRAW::NO_DEPTH);


    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(50.0f, 10.0f, 0.0f);
    tc->GetTransform()->SetScale(10, 1, 50);
    tc->GetTransform()->SetRotationZ(-PI / 2);
    tc->GetTransform()->SetRotationY(PI);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0f, 0.0f, 1.0f);

    // Top Wall
    entity = scene->AddEntity("wallTop");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(fenceModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE | FLAG_DRAW::NO_DEPTH);


    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 10.0f, 50.0f);
    tc->GetTransform()->SetScale(10, 1, 50);
    tc->GetTransform()->SetRotationZ(PI / 2);
    tc->GetTransform()->SetRotationX(PI / 2);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0f, 0.0f, 1.0f);

    // Bot Wall
    entity = scene->AddEntity("wallBot");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(fenceModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_TRANSPARENT_TEXTURE | FLAG_DRAW::NO_DEPTH | FLAG_DRAW::GIVE_SHADOW);


    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 10.0f, -50.0f);
    tc->GetTransform()->SetScale(10, 1, 50);
    tc->GetTransform()->SetRotationZ(PI / 2);
    tc->GetTransform()->SetRotationX(PI / 2);
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0f, 0.0f, 1.0f);

#pragma endregion walls
    /* ---------------------- SpotLightDynamic ---------------------- */
    entity = scene->AddEntity("spotLightDynamic");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    slc = entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::CAST_SHADOW | FLAG_LIGHT::STATIC);

    float3 pos = { 5.0f, 20.0f, 5.0f };
    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(pos.x, pos.y, pos.z);

    slc->SetColor({ 5.0f, 0.0f, 0.0f });
    slc->SetAttenuation({ 1.0, 0.09f, 0.032f });
    slc->SetPosition(pos);
    slc->SetDirection({ 1.0f, -1.0f, 1.0f });
    slc->SetOuterCutOff(50.0f);
    /* ---------------------- SpotLightDynamic ---------------------- */

    /* ---------------------- dirLight ---------------------- */
    entity = scene->AddEntity("dirLight");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::STATIC | FLAG_LIGHT::CAST_SHADOW);
    dlc->SetColor({ 0.8f, 0.8f, 0.8f });
    dlc->SetDirection({ -2.0f, -1.0f, -1.0f });
    dlc->SetCameraTop(50.0f);
    dlc->SetCameraBot(-30.0f);
    dlc->SetCameraLeft(-70.0f);
    dlc->SetCameraRight(70.0f);
    /* ---------------------- dirLight ---------------------- */

    /* ---------------------- Update Function ---------------------- */
    UpdateScene = &ShopUpdateScene;
    return scene;
}

void DefaultUpdateScene(SceneManager* sm, double dt)
{
}

void DemoUpdateScene(SceneManager* sm, double dt)
{
    component::Audio3DEmitterComponent* ec;

    std::string name = "Enemy";
    for (int i = 0; i < 75; i++)
    {
        name = "Enemy" + std::to_string(i);
        ec = sm->GetScene("DemoScene")->GetEntity(name)->GetComponent<component::Audio3DEmitterComponent>();
        ec->UpdateEmitter(L"Bruh");
    }
}

void ShopUpdateScene(SceneManager* sm, double dt)
{
    static float rotValue = 0.0f;
    Transform* trans = sm->GetScene("shopScene")->GetEntity("poster")->GetComponent<component::TransformComponent>()->GetTransform();
    trans->SetRotationX(rotValue);

    rotValue += 0.005f;
}