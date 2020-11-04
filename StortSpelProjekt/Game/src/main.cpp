#include "Engine.h"
#include "EnemyFactory.h"
#include "GameNetwork.h"
#include "GameGUI.h"
#include "Physics/CollisionCategories/PlayerCollisionCategory.h"
#include "Physics/CollisionCategories/PlayerProjectileCollisionCategory.h"
// Game includes
#include "Player.h"
#include "UpgradeManager.h"
#include "Shop.h"
#include "Components/CurrencyComponent.h"

Scene* GameScene(SceneManager* sm);
Scene* ShopScene(SceneManager* sm);
Scene* GameOverScene(SceneManager* sm);

void GameUpdateScene(SceneManager* sm, double dt);
void ShopUpdateScene(SceneManager* sm, double dt);

EnemyFactory enemyFactory;
GameGUI gameGUI;

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
    ParticleSystem* const particleSystem = engine.GetParticleSystem();


    /*------ AssetLoader to load models / textures ------*/
    AssetLoader* al = AssetLoader::Get();

    /*----- Set the scene -----*/
    Scene* demoScene = GameScene(sceneManager);
    Scene* shopScene = ShopScene(sceneManager);
    Scene* gameOverScene = GameOverScene(sceneManager);

    //Scene* shopScene = ShopScene(sceneManager);
    sceneManager->SetScenes(demoScene);
    sceneManager->SetGameOverScene(gameOverScene);

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

        sceneManager->ChangeScene();

        logicTimer += timer->GetDeltaTime();
        if (gameNetwork.IsConnected())
        {
            networkTimer += timer->GetDeltaTime();
        }

        sceneManager->RenderUpdate(timer->GetDeltaTime());
        particleSystem->Update(timer->GetDeltaTime());
        if (logicTimer >= updateRate)
        {
            logicTimer = 0;
            sceneManager->Update(updateRate);
            physics->Update(updateRate);
            enemyFactory.Update(updateRate);
            gameGUI.Update(updateRate, sceneManager->GetActiveScenes()->at(0));
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
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Female/female4armor.obj");    
    Model* enemyModel = al->LoadModel(L"../Vendor/Resources/Models/Zombie/zombie.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* teleportModel = al->LoadModel(L"../Vendor/Resources/Models/Teleporter/Teleporter.obj");

    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    AudioBuffer* projectileSound = al->LoadAudio(L"../Vendor/Resources/Audio/fireball.wav", L"Fireball");
    AudioBuffer* swordSwing = al->LoadAudio(L"../Vendor/Resources/Audio/swing_sword.wav", L"SwordSwing");

    /*--------------------- Assets ---------------------*/

    /*--------------------- Component declarations ---------------------*/
    Entity* entity = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::Audio3DListenerComponent* alc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PlayerInputComponent* pic = nullptr;
    component::GUI2DComponent* txc = nullptr;
    component::TeleportComponent* teleC = nullptr;
    component::CollisionComponent* ccc = nullptr;
    component::SphereCollisionComponent* scc = nullptr;
    component::MeleeComponent* melc = nullptr;
    component::RangeComponent* ranc = nullptr;
    component::CurrencyComponent* currc = nullptr;
    component::HealthComponent* hc = nullptr;
    component::UpgradeComponent* uc = nullptr;
    component::GUI2DComponent* gui = nullptr;
    /*--------------------- Component declarations ---------------------*/

    /*--------------------- Player ---------------------*/
    // entity
    std::string playerName = "player";
    entity = scene->AddEntity(playerName);

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    pic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    alc = entity->AddComponent<component::Audio3DListenerComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    melc = entity->AddComponent<component::MeleeComponent>();
    // range damage should be at least 10 for ranged life steal upgrade to work
    // range velocity should be 50, otherwise range velocity upgrade does not make sense (may be scrapped later)
    ranc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.4, 10, 50);
    currc = entity->AddComponent<component::CurrencyComponent>();
    hc = entity->AddComponent<component::HealthComponent>(50);
    uc = entity->AddComponent<component::UpgradeComponent>();
    alc = entity->AddComponent<component::Audio3DListenerComponent>();

    Player::GetInstance().SetPlayer(entity);

    tc->GetTransform()->SetScale(0.9f);
    tc->GetTransform()->SetPosition(0.0f, 1.0f, 0.0f);
    tc->SetTransformOriginalState();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    
    double3 playerDim = mc->GetModelDim();

    double rad = playerDim.z / 2.0;
    double cylHeight = playerDim.y - (rad * 2.0);
    ccc = entity->AddComponent<component::CapsuleCollisionComponent>(200.0, rad, cylHeight, 0.0, 0.0, false);

    melc->SetDamage(10);
    melc->setAttackInterval(0.8);
    ranc->SetAttackInterval(0.8);
    pic->Init();
    pic->SetJumpTime(0.17);
    pic->SetJumpHeight(6.0);
	pic->SetMovementSpeed(70.0);

    avc->AddVoice(L"Bruh");

    bbc->Init();
    bbc->AddCollisionCategory<PlayerCollisionCategory>();
    Physics::GetInstance().AddCollisionEntity(entity);;
    /*--------------------- Player ---------------------*/

    /*--------------------- DirectionalLight ---------------------*/
    entity = scene->AddEntity("sun");

    // components
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW);
    dlc->SetDirection({ 0.05f, -0.3f, 0.5f });
    dlc->SetColor({ 252.0f / 256.0f, 156.0f / 256.0f, 84.0f / 256.0f });
    dlc->SetCameraTop(150.0f);
    dlc->SetCameraBot(-120.0f);
    dlc->SetCameraRight(130.0f);
    dlc->SetCameraLeft(-180.0f);
    dlc->SetCameraNearZ(-1000.0f);
    /*--------------------- DirectionalLight ---------------------*/

	EnemyComps zombie = {};
	zombie.model = enemyModel;
	zombie.hp = 30;
	zombie.sound3D = L"Bruh";
	zombie.compFlags = F_COMP_FLAGS::OBB | F_COMP_FLAGS::CAPSULE_COLLISION;
	zombie.aiFlags = 0;
	zombie.meleeAttackDmg = 5.0f;
	zombie.attackInterval = 1.5f;
	zombie.attackSpeed = 0.1f;
	zombie.movementSpeed = 30.0f;
	zombie.attackingDist = 1.5f;
	zombie.rot = { 0.0, 0.0, 0.0 };
	zombie.targetName = "player";
	zombie.scale = 0.04;
	zombie.detectionRad = 500.0f;

    /* ---------------------- Teleporter ---------------------- */
    entity = scene->AddEntity("teleporter");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    teleC = entity->AddComponent<component::TeleportComponent>(scene->GetEntity(playerName), "ShopScene");


    mc->SetModel(teleportModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetPosition(1000.0f, 1000.0f, 1000.0f);
    tc->GetTransform()->SetScale(7.0f);
    tc->SetTransformOriginalState();

    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);
    /*--------------------- Teleporter ---------------------*/
    /* ------------------------- GUI --------------------------- */
    std::string textToRender = "HEALTH";
    float2 textPos = { 0.45f, 0.96f };
    float2 textPadding = { 0.5f, 0.0f };
    float4 textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float2 textScale = { 0.5f, 0.5f };
    float4 textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

    entity = scene->AddEntity("health");
    gui = entity->AddComponent<component::GUI2DComponent>();
    gui->GetTextManager()->AddText("health");
    gui->GetTextManager()->SetColor(textColor, "health");
    gui->GetTextManager()->SetPadding(textPadding, "health");
    gui->GetTextManager()->SetPos(textPos, "health");
    gui->GetTextManager()->SetScale(textScale, "health");
    gui->GetTextManager()->SetText(textToRender, "health");
    gui->GetTextManager()->SetBlend(textBlend, "health");

    float2 quadPos = { 0.4f, 0.95f };
    float2 quadScale = { 0.2f, 0.1f };
    float4 blended = { 1.0, 1.0, 1.0, 0.99 };
    float4 notBlended = { 1.0, 1.0, 1.0, 1.0 };
    gui->GetQuadManager()->CreateQuad(
        "health",
        quadPos, quadScale,
        false, false,
        1,
        notBlended,
        nullptr,
        { 0.0, 1.0, 0.0 }
    );

    textToRender = "Currency: 0";
    textPos = { 0.01f, 0.95f };
    textPadding = { 0.5f, 0.0f };
    textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    textScale = { 0.5f, 0.5f };
    textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

    entity = scene->AddEntity("money");
    gui = entity->AddComponent<component::GUI2DComponent>();
    gui->GetTextManager()->AddText("money");
    gui->GetTextManager()->SetColor(textColor, "money");
    gui->GetTextManager()->SetPadding(textPadding, "money");
    gui->GetTextManager()->SetPos(textPos, "money");
    gui->GetTextManager()->SetScale(textScale, "money");
    gui->GetTextManager()->SetText(textToRender, "money");
    gui->GetTextManager()->SetBlend(textBlend, "money");

    textToRender = "Enemies: 0/20";
    textPos = { 0.01f, 0.1f };
    textPadding = { 0.5f, 0.0f };
    textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    textScale = { 0.5f, 0.5f };
    textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

    entity = scene->AddEntity("enemyGui");
    gui = entity->AddComponent<component::GUI2DComponent>();
    gui->GetTextManager()->AddText("enemyGui");
    gui->GetTextManager()->SetColor(textColor, "enemyGui");
    gui->GetTextManager()->SetPadding(textPadding, "enemyGui");
    gui->GetTextManager()->SetPos(textPos, "enemyGui");
    gui->GetTextManager()->SetScale(textScale, "enemyGui");
    gui->GetTextManager()->SetText(textToRender, "enemyGui");
    gui->GetTextManager()->SetBlend(textBlend, "enemyGui");

    /* ---------------------------------------------------------- */

#pragma region Enemyfactory
    enemyFactory.SetScene(scene);
    enemyFactory.AddSpawnPoint({ 70, 5, 20 });
    enemyFactory.AddSpawnPoint({ -20, 5, -190 });
    enemyFactory.AddSpawnPoint({ -120, 10, 75 });
    enemyFactory.DefineEnemy("enemyZombie", &zombie);
#pragma endregion

    scene->SetCollisionEntities(Physics::GetInstance().GetCollisionEntities());
    Physics::GetInstance().OnResetScene();

    scene->SetUpdateScene(&GameUpdateScene);

    return scene;
}

Scene* GameOverScene(SceneManager* sm)
{
    AssetLoader* al = AssetLoader::Get();

    // Create Scene
    Scene* scene = sm->CreateScene("gameOverScene");

    // Player (Need a camera)
    Entity* entity = scene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);

    // Skybox
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    TextureCubeMap* blackCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/black.dds");
    sbc->SetTexture(blackCubeMap);

    // Game over Text
    Entity* text = scene->AddEntity("gameOverText");
    component::GUI2DComponent* textComp = text->AddComponent<component::GUI2DComponent>();
    textComp->GetTextManager()->AddText("GameOverText");
    textComp->GetTextManager()->SetScale({2, 2}, "GameOverText");
    textComp->GetTextManager()->SetPos({0.29, 0.41}, "GameOverText");
    textComp->GetTextManager()->SetText("Game Over", "GameOverText");

    // text2
    Entity* text2 = scene->AddEntity("youDiedText");
    component::GUI2DComponent* textComp2 = text2->AddComponent<component::GUI2DComponent>();
    textComp->GetTextManager()->AddText("youDiedText");
    textComp->GetTextManager()->SetScale({ 0.6, 0.6 }, "youDiedText");
    textComp->GetTextManager()->SetPos({ 0.43, 0.56 }, "youDiedText");
    textComp->GetTextManager()->SetText("(You Died...)", "youDiedText");
    
    return scene;
}

Scene* ShopScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("ShopScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PlayerInputComponent* ic = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::CollisionComponent* bcc = nullptr;
    component::TeleportComponent* teleC = nullptr;
    component::MeleeComponent* mac = nullptr;
    component::RangeComponent* rc = nullptr;
    component::UpgradeComponent* uc = nullptr;
    component::CapsuleCollisionComponent* ccc = nullptr;
    component::HealthComponent* hc = nullptr;
    component::CurrencyComponent* cur = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Female/female4armor.obj");
    Model* shopModel = al->LoadModel(L"../Vendor/Resources/Models/Shop/shop.obj");
    Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");
    Model* fenceModel = al->LoadModel(L"../Vendor/Resources/Models/FencePBR/fence.obj");
    Model* teleportModel = al->LoadModel(L"../Vendor/Resources/Models/Teleporter/Teleporter.obj");

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
    cur = entity->AddComponent<component::CurrencyComponent>();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(0.9f);
    tc->GetTransform()->SetPosition(0.0, 20.0, 0.0);
    tc->SetTransformOriginalState();

    double3 playerDim = mc->GetModelDim();

    double rad = playerDim.z / 2.0;
    double cylHeight = playerDim.y - (rad * 2.0);
    ccc = entity->AddComponent<component::CapsuleCollisionComponent>(200.0, rad, cylHeight, 0.0, 0.0, false);
    hc = entity->AddComponent<component::HealthComponent>(10000000);
    ic->SetMovementSpeed(70.0);
    ic->Init();
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);

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
    tc->SetTransformOriginalState();
    /* ---------------------- Floor ---------------------- */

    /* ---------------------- Teleporter ---------------------- */
    entity = scene->AddEntity("teleporter");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    teleC = entity->AddComponent<component::TeleportComponent>(scene->GetEntity(playerName), "GameScene");
    
    mc->SetModel(teleportModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetPosition(-10.0f, 1.0f, -25.0f);
    tc->GetTransform()->SetScale(7.0f);
    tc->SetTransformOriginalState();
    
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
    tc->SetTransformOriginalState();
    /* ---------------------- Poster ---------------------- */

    /* ---------------------- Shop ---------------------- */
    entity = scene->AddEntity("shop");
    mc = entity->AddComponent<component::ModelComponent>();
    mc->SetModel(shopModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);

    tc = entity->AddComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(30.0f, 0.0f, 30.0f);
    tc->GetTransform()->SetRotationY(PI + PI / 4);
    tc->SetTransformOriginalState();

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
    tc->SetTransformOriginalState();
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
    tc->SetTransformOriginalState();
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
    tc->SetTransformOriginalState();
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
    tc->SetTransformOriginalState();
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
    tc->SetTransformOriginalState();

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

void GameUpdateScene(SceneManager* sm, double dt)
{
    if (ImGuiHandler::GetInstance().GetBool("reset"))
    {
        ImGuiHandler::GetInstance().SetBool("reset", false);
    }
}

void ShopUpdateScene(SceneManager* sm, double dt)
{
    static float rotValue = 0.0f;
    Transform* trans = sm->GetScene("ShopScene")->GetEntity("poster")->GetComponent<component::TransformComponent>()->GetTransform();
    trans->SetRotationX(rotValue);

    rotValue += 0.005f;
}