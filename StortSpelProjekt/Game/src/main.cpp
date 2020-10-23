#include "Engine.h"
#include "EnemyFactory.h"
#include "GameNetwork.h"

// Game includes
#include "Player.h"
#include "Shop.h"

Scene* GameScene(SceneManager* sm);
Scene* ShopScene(SceneManager* sm);

void DefaultUpdateScene(SceneManager* sm, double dt);
void GameUpdateScene(SceneManager* sm, double dt);
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

    /*----- Set the scene -----*/
    Scene* gameScene = GameScene(sceneManager);
    Scene* shopScene = ShopScene(sceneManager);

    //Scene* shopScene = ShopScene(sceneManager);
    sceneManager->SetScenes(1, &gameScene);

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
        // Check if player walks into portal and should be teleported
        sceneManager->ChangeSceneIfTeleported();

        /* ------ Update ------ */
        timer->Update();
        
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

Scene* GameScene(SceneManager* sm)
{
    Scene* scene = sm->CreateScene("GameScene");

    AssetLoader* al = AssetLoader::Get();

    al->LoadMap(scene, "../Vendor/Resources/FirstMap.txt");
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Man/man.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/CubePBR/cube.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    AudioBuffer* projectileSound = al->LoadAudio(L"../Vendor/Resources/Audio/fireball.wav", L"Fireball");
    AudioBuffer* swordSwing = al->LoadAudio(L"../Vendor/Resources/Audio/swing_sword.wav", L"SwordSwing");

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
    component::GUI2DComponent* txc = nullptr;
    component::CollisionComponent* ccc = nullptr;
    component::CubeCollisionComponent* bcc = nullptr;
    component::SphereCollisionComponent* scc = nullptr;
    component::TeleportComponent* teleC = nullptr;
    component::MeleeComponent* melc = nullptr;
    component::RangeComponent* ranc = nullptr;
    /*--------------------- Component declarations ---------------------*/

    /*--------------------- Player ---------------------*/
    std::string playerName = "player";
    entity = scene->AddEntity(playerName);

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    pic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    melc = entity->AddComponent<component::MeleeComponent>();
    ranc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.2, 1, 50);

    tc->GetTransform()->SetScale(0.5f);
    scene->SetOriginalPosition(0.0f, 1.0f, 0.0f);
    tc->GetTransform()->SetPosition(0.0f, 1.0f, 0.0f);

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);

    ccc = entity->AddComponent<component::CubeCollisionComponent>(1, mc->GetModelDim().x / 2.0f, mc->GetModelDim().y / 2.0f, mc->GetModelDim().z / 2.0f, 0, 0, false);
    pic->Init();
    pic->SetJumpTime(0.17);
    pic->SetJumpHeight(6.0);

    avc->AddVoice(L"Bruh");

    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);
    /*--------------------- Player ---------------------*/

    /*--------------------- DirectionalLight ---------------------*/
    entity = scene->AddEntity("sun");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetDirection({ 0.05f, -0.3f, 0.5f });
    dlc->SetColor({ 252.0f / 256.0f, 156.0f / 256.0f, 84.0f / 256.0f });
    dlc->SetCameraTop(150.0f);
    dlc->SetCameraBot(-120.0f);
    dlc->SetCameraRight(130.0f);
    dlc->SetCameraLeft(-180.0f);
    dlc->SetCameraNearZ(-1000.0f);
    /*--------------------- DirectionalLight ---------------------*/

    /* ---------------------- Teleporter ---------------------- */
    entity = scene->AddEntity("teleporter");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0, 1.0, 1.0);
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    teleC = entity->AddComponent<component::TeleportComponent>(scene->GetEntity(playerName), "shopScene");

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(-50.0f, 5.0f, -25.0f);
    tc->GetTransform()->SetScale(5.0f, 5.0f, 1.0f);
    tc->GetTransform()->SetRotationY(PI / 2);

    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);
    /* ---------------------- Teleporter ---------------------- */

    scene->SetCollisionEntities(Physics::GetInstance().GetCollisionEntities());
    Physics::GetInstance().OnResetScene();

    scene->SetUpdateScene(&GameUpdateScene);

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
    component::TeleportComponent* teleC = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Man/man.obj");
    Model* shopModel = al->LoadModel(L"../Vendor/Resources/Models/Shop/shop.obj");
    Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");
    Model* fenceModel = al->LoadModel(L"../Vendor/Resources/Models/FencePBR/fence.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/CubePBR/cube.obj");

    TextureCubeMap* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");

#pragma region player
    std::string playerName = "player";
    Entity* entity = scene->AddEntity(playerName);
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
    tc->GetTransform()->SetScale(0.5f);
    scene->SetOriginalPosition(0.0, 20.0, 0.0);
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

    /* ---------------------- Teleporter ---------------------- */
    entity = scene->AddEntity("teleporter");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 1.0, 1.0, 1.0);
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    teleC = entity->AddComponent<component::TeleportComponent>(scene->GetEntity(playerName), "GameScene");
    
    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(-10.0f, 5.0f, -25.0f);
    tc->GetTransform()->SetScale(5.0f, 5.0f, 1.0f);
    tc->GetTransform()->SetRotationY(PI / 2);
    
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);
    /* ---------------------- Teleporter ---------------------- */

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

    scene->SetCollisionEntities(Physics::GetInstance().GetCollisionEntities());
    Physics::GetInstance().OnResetScene();
    /* ---------------------- Update Function ---------------------- */
    scene->SetUpdateScene(&ShopUpdateScene);
    return scene;
}

void DefaultUpdateScene(SceneManager* sm, double dt)
{
}

void GameUpdateScene(SceneManager* sm, double dt)
{
    
}

void ShopUpdateScene(SceneManager* sm, double dt)
{
    static float rotValue = 0.0f;
    Transform* trans = sm->GetScene("shopScene")->GetEntity("poster")->GetComponent<component::TransformComponent>()->GetTransform();
    trans->SetRotationX(rotValue);

    rotValue += 0.005f;
}
