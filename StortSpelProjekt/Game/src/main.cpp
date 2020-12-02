#include "Engine.h"
#include "EnemyFactory.h"
#include "GameNetwork.h"
#include "GameGUI.h"
#include "Physics/CollisionCategories/PlayerCollisionCategory.h"
#include "Physics/CollisionCategories/PlayerProjectileCollisionCategory.h"
#include "../ECS/Components/TemporaryLifeComponent.h"

// Game includes
#include "Player.h"
#include "UpgradeManager.h"
#include "Shop.h"
#include "Components/CurrencyComponent.h"
#include "Components/UpgradeComponents/UpgradeComponent.h"
#include "MainMenuHandler.h"
#include "GameOverHandler.h"
#include "UpgradeGUI.h"

#include "Misc/Edge.h"

//#include "Misc/Cryptor.h"

Scene* LoadScene(SceneManager* sm);
Scene* GameScene(SceneManager* sm);
Scene* ShopScene(SceneManager* sm);

void GameInitScene(Scene* scene);
void ShopInitScene(Scene* scene);
void ParticleInit();
void GameUpdateScene(SceneManager* sm, double dt);
void ShopUpdateScene(SceneManager* sm, double dt);

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
    LoadScene(sceneManager);
    Scene* demoScene = GameScene(sceneManager);
    Scene* shopScene = ShopScene(sceneManager);
    Scene* gameOverScene = GameOverHandler::GetInstance().CreateScene(sceneManager);
    Scene* mainMenuScene = MainMenuHandler::GetInstance().CreateScene(sceneManager);
    sceneManager->SetScene(mainMenuScene);
    sceneManager->SetGameOverScene(gameOverScene);
    GameNetwork gameNetwork;

    /*-------- UpgradeGUI ---------*/
    UpgradeGUI::GetInstance().Init();

    /*------ Network Init -----*/

    if (std::atoi(option->GetVariable("i_network").c_str()) == 1)
    {
        gameNetwork.SetScene(sceneManager->GetActiveScene());
        gameNetwork.SetSceneManager(sceneManager);
        gameNetwork.SetEnemies(EnemyFactory::GetInstance().GetAllEnemies());
    }
    double networkTimer = 0;
    double logicTimer = 0;
    int count = 0;

    

    while (!window->ExitWindow())
    {
        /* ------ Update ------ */
        timer->Update();
        logicTimer += timer->GetDeltaTime();

        bool changedScene = sceneManager->ChangeScene();
        if(changedScene)
        {
            // if change scene, reset dt
            timer->StartTimer();
            timer->Update();
        }

        
        if (gameNetwork.IsConnected())
        {
            networkTimer += timer->GetDeltaTime();
        }

        sceneManager->RenderUpdate(timer->GetDeltaTime());
        particleSystem->Update(timer->GetDeltaTime());
        if (logicTimer >= updateRate)
        {
            if (logicTimer >= 0.5)
            {
                logicTimer = 0;
            }
            else
            {
                logicTimer -= updateRate;
            }
            sceneManager->Update(updateRate);
            physics->Update(updateRate);
            EnemyFactory::GetInstance().Update(updateRate);
            gameGUI.Update(updateRate, sceneManager->GetActiveScene());
            UpgradeGUI::GetInstance().Update(updateRate, sceneManager->GetActiveScene());
        }

        /* ---- Network ---- */
        if (gameNetwork.IsConnected())
        {
            if (networkTimer >= networkUpdateRate) 
			{
                networkTimer = 0;

                gameNetwork.Update(networkUpdateRate);
            }
        }

        /* ------ Sort ------ */
        renderer->SortObjects();

        /* ------ Draw ------ */
        renderer->Execute();

        sceneManager->RemoveEntities();
    }
    return 0;
}

//This scene is only used to load assets!
Scene* LoadScene(SceneManager* sm)
{
    Scene* scene = sm->CreateScene("LoadScene");
    std::vector<float3> spawnPoints;
    AssetLoader::Get()->GenerateMap(scene, "../Vendor/Resources/Rooms", &spawnPoints, { 3.0f,3.0f }, { 173.0f,200.0f }, false);
    return scene;
}

Scene* GameScene(SceneManager* sm)
{
    Scene* scene = sm->CreateScene("GameScene");

#pragma region assets
    AssetLoader* al = AssetLoader::Get();

    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/IgnoredModels/Player/AnimatedPlayer.fbx");
    Model* enemyZombieModel = al->LoadModel(L"../Vendor/Resources/Models/Zombie/AnimatedZombie.fbx");
    Model* enemySpiderModel = al->LoadModel(L"../Vendor/Resources/Models/IgnoredModels/Spider/AnimatedSpider.fbx");
    Model* enemyDemonModel = al->LoadModel(L"../Vendor/Resources/Models/IgnoredModels/Demon/AnimatedDemon.fbx");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/CubePBR/cube.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* teleportModel = al->LoadModel(L"../Vendor/Resources/Models/Teleporter/Teleporter.obj");

    Texture* currencyIcon = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/currency.png");

    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    AudioBuffer* playerHit1 = al->LoadAudio(L"../Vendor/Resources/Audio/Femalegrunt.wav", L"PlayerHit1");
    AudioBuffer* projectileSound = al->LoadAudio(L"../Vendor/Resources/Audio/fireball.wav", L"Fireball");
    AudioBuffer* swordSwing = al->LoadAudio(L"../Vendor/Resources/Audio/swing_sword.wav", L"SwordSwing");
    AudioBuffer* demonGnarl1 = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/Demon_Vocalisation_1.wav", L"DemonGnarl1");
    AudioBuffer* demonGnarl2 = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/Demon_Vocalisation_2.wav", L"DemonGnarl2");
    AudioBuffer* demonGnarl3 = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/Demon_Vocalisation_3.wav", L"DemonGnarl3");
    AudioBuffer* demonGnarl4 = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/Demon_Vocalisation_4.wav", L"DemonGnarl4");
    AudioBuffer* demonGnarl5 = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/Demon_Vocalisation_5.wav", L"DemonGnarl5");
    AudioBuffer* demonGnarl6 = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/Demon_Vocalisation_6.wav", L"DemonGnarl6");
    AudioBuffer* demonGnarl7 = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/Demon_Vocalisation_7.wav", L"DemonGnarl7");
    AudioBuffer* demonHit7 = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/Demon_Vocalisation_GotHit_7.wav", L"DemonHit7");
    AudioBuffer* spiderCrawl = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/spiderCrawl.wav", L"SpiderCrawl");
    spiderCrawl->SetAudioLoop(0);
    AudioBuffer* spiderScream = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/Spider_DeathScream_2.wav", L"SpiderHit");
    AudioBuffer* spiderSound = al->LoadAudio(L"../Vendor/Resources/Audio/IgnoredAudio/spiderSound.wav", L"SpiderSound");

	Texture* healthBackgroundTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HealthBackground.png");
	Texture* healthbarTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Healthbar.png");
	Texture* healthGuardiansTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HealthGuardians.png");
	Texture* healthHolderTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HealthHolder.png");
	Texture* crosshairTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Crosshair.png");
	Texture* killedEnemiesHolderTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/KilledEnemies.png");

	Font* arial = al->LoadFontFromFile(L"Arial.fnt");

#pragma endregion

#pragma region component declarations
    Entity* entity = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::Audio3DListenerComponent* alc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::AnimationComponent* ac = nullptr;
    component::PointLightComponent* plc = nullptr;
	component::SpotLightComponent* slc = nullptr;
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
    component::ParticleEmitterComponent* pec = nullptr;
#pragma endregion

#pragma region entities
#pragma region player
    // entity
    std::string playerName = "player";
    entity = scene->AddEntity(playerName);

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    ac = entity->AddComponent<component::AnimationComponent>();
    tc = entity->AddComponent<component::TransformComponent>(true);
    pic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    alc = entity->AddComponent<component::Audio3DListenerComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION | F_OBBFlags::T_POSE);
    // range damage should be at least 10 for ranged life steal upgrade to work
    ranc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.4, 50, 150);
    currc = entity->AddComponent<component::CurrencyComponent>();
    hc = entity->AddComponent<component::HealthComponent>(500);
    uc = entity->AddComponent<component::UpgradeComponent>();
    alc = entity->AddComponent<component::Audio3DListenerComponent>();

    Player::GetInstance().SetPlayer(entity);

    tc->GetTransform()->SetScale(0.05f);
    tc->GetTransform()->SetPosition(0.0f, 1.0f, 0.0f);
    tc->SetTransformOriginalState();

    melc = entity->AddComponent<component::MeleeComponent>();   // moved this down to set scale first

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_ANIMATED | FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::NO_DEPTH);
    ac->Initialize();

    double3 playerDim = mc->GetModelDim();

    double rad = playerDim.z / 2.0;
    double cylHeight = playerDim.y - (rad * 2.0);
    ccc = entity->AddComponent<component::CapsuleCollisionComponent>(200.0, rad, cylHeight, 0.0, 0.0, false);

    melc->SetDamage(50);
    melc->SetAttackInterval(1.0);
    ranc->SetAttackInterval(1.0);
    pic->Init();
    pic->SetJumpTime(0.17);
    pic->SetJumpHeight(6.0);
	pic->SetMovementSpeed(75.0);

    avc->AddVoice(L"PlayerHit1");

    bbc->Init();
    bbc->AddCollisionCategory<PlayerCollisionCategory>();
    Physics::GetInstance().AddCollisionEntity(entity);
#pragma endregion

#pragma region enemy definitions
    // melee
	EnemyComps zombie = {};
	zombie.model = enemyZombieModel;
	zombie.hp = 70;
	zombie.hpBase = 70;
	zombie.OnHitSounds.emplace_back(L"DemonHit7");
    zombie.OnGruntSounds.emplace_back(L"DemonGnarl1");
    zombie.OnGruntSounds.emplace_back(L"DemonGnarl2");
    zombie.OnGruntSounds.emplace_back(L"DemonGnarl3");
    zombie.OnGruntSounds.emplace_back(L"DemonGnarl4");
    zombie.OnGruntSounds.emplace_back(L"DemonGnarl5");
    zombie.OnGruntSounds.emplace_back(L"DemonGnarl6");
    zombie.OnGruntSounds.emplace_back(L"DemonGnarl7");
	zombie.compFlags = F_COMP_FLAGS::OBB | F_COMP_FLAGS::CAPSULE_COLLISION;
	zombie.aiFlags = 0;
	zombie.meleeAttackDmg = 30.0f;
	zombie.meleeAttackDmgBase = 30.0f;
	zombie.attackInterval = 1.5f;
	zombie.attackSpeed = 0.1f;
	zombie.movementSpeed = 45.0f;
	zombie.rot = { 0.0, 0.0, 0.0 };
	zombie.targetName = "player";
	zombie.scale = 0.014;
	zombie.detectionRad = 500.0f;
	zombie.attackingDist = 1.5f;
    zombie.invertDirection = true;
    zombie.mass = 150.0f;
    zombie.slowAttack = 0.5f;

    // quick melee
    EnemyComps spider = {};
    spider.model = enemySpiderModel;
    spider.hp = 40;
    spider.hpBase = 40;
    spider.OnHitSounds.emplace_back(L"SpiderHit");
    spider.OnGruntSounds.emplace_back(L"SpiderSound");
    spider.compFlags = F_COMP_FLAGS::OBB | F_COMP_FLAGS::CAPSULE_COLLISION;
    spider.aiFlags = F_AI_FLAGS::RUSH_PLAYER;
    spider.meleeAttackDmg = 15.0f;
    spider.meleeAttackDmgBase = 15.0f;
    spider.attackInterval = 0.70f;
    spider.attackSpeed = 0.05f;
    spider.movementSpeed = 90.0f;
    spider.rot = { 0.0, 0.0, 0.0 };
    spider.targetName = "player";
    spider.scale = 0.013;
    spider.detectionRad = 500.0f;
    spider.attackingDist = 1.5f;
    spider.invertDirection = true;
    spider.mass = 100.0f;
    spider.walkSound = L"SpiderCrawl";

    // ranged
    EnemyComps rangedDemon = {};
    rangedDemon.model = enemyDemonModel;
    rangedDemon.hp = 120;
    rangedDemon.hpBase = 120;
    rangedDemon.OnHitSounds.emplace_back(L"Bruh");
    rangedDemon.compFlags = F_COMP_FLAGS::OBB | F_COMP_FLAGS::CAPSULE_COLLISION;
    rangedDemon.aiFlags = F_AI_FLAGS::RUSH_PLAYER;
    rangedDemon.attackInterval = 2.5f;
    rangedDemon.attackSpeed = 1.0f;
    rangedDemon.movementSpeed = 30.0f;
    rangedDemon.targetName = "player";
    rangedDemon.scale = 0.08f;
    rangedDemon.isRanged = true;
    rangedDemon.detectionRad = 500.0f;
    rangedDemon.attackingDist = 100.0f;
    rangedDemon.rangeAttackDmg = 70;
    rangedDemon.rangeVelocity = 50.0f;
    rangedDemon.projectileModel = sphereModel;
    rangedDemon.invertDirection = true;
    rangedDemon.mass = 300.0f;

#pragma endregion

#pragma region Enemyfactory
    EnemyFactory::GetInstance().SetScene(scene);

    EnemyFactory::GetInstance().DefineEnemy("enemyZombie", &zombie);
    EnemyFactory::GetInstance().DefineEnemy("enemySpider", &spider);
    EnemyFactory::GetInstance().DefineEnemy("enemyDemon", &rangedDemon);
#pragma endregion

#pragma region teleporter
    entity = scene->AddEntity("teleporter");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    teleC = entity->AddComponent<component::TeleportComponent>(scene->GetEntity(playerName), "ShopScene");

    // Create test particleEffect
    ParticleEffectSettings settings = {};
    settings.maxParticleCount = 100;
    settings.startValues.lifetime = 0.8;
    settings.spawnInterval = settings.startValues.lifetime / settings.maxParticleCount;
    settings.startValues.acceleration = { 0, 0, 0 };

    // Need to fix EngineRand.rand() for negative values

    settings.randPosition = { -6, 6, 0, 15, -6, 6 };
    settings.randVelocity = { -2, 2, 0, 2, -2, 2 };
    settings.randSize = { 0.3, 0.9 };
    settings.randRotationSpeed = { 0, 1 };

    Texture2DGUI* particleTexture = static_cast<Texture2DGUI*>(al->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/portal_particle_blue.png"));
    settings.texture = particleTexture;
    pec = entity->AddComponent<component::ParticleEmitterComponent>(&settings, true);


    mc->SetModel(teleportModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetPosition(1000.0f, 1000.0f, 1000.0f);
    tc->GetTransform()->SetScale(7.0f);
    tc->SetTransformOriginalState();

    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);
#pragma endregion
#pragma endregion

#pragma region GUI
#pragma region health background
	std::string textToRender = "";
	float2 textPos = { 0.473f, 0.965f };
	float2 textPadding = { 0.8f, 0.0f };
	float4 textColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	float2 textScale = { 0.3f, 0.3f };
	float4 textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

	entity = scene->AddEntity("healthBackground");
	gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetTextManager()->SetFont(arial);
	gui->GetTextManager()->AddText("currentHealth");
	gui->GetTextManager()->SetColor(textColor, "currentHealth");
	gui->GetTextManager()->SetPadding(textPadding, "currentHealth");
	gui->GetTextManager()->SetPos(textPos, "currentHealth");
	gui->GetTextManager()->SetScale(textScale, "currentHealth");
	gui->GetTextManager()->SetText(std::to_string(hc->GetHealth()), "currentHealth");
	gui->GetTextManager()->SetBlend(textBlend, "currentHealth");

	textPos = { 0.499f, 0.965f };
	gui->GetTextManager()->AddText("slash");
	gui->GetTextManager()->SetColor(textColor, "slash");
	gui->GetTextManager()->SetPadding(textPadding, "slash");
	gui->GetTextManager()->SetPos(textPos, "slash");
	gui->GetTextManager()->SetScale(textScale, "slash");
	gui->GetTextManager()->SetText("/", "slash");
	gui->GetTextManager()->SetBlend(textBlend, "slash");

	textPos = { 0.503f, 0.965f };
	gui->GetTextManager()->AddText("maxHealth");
	gui->GetTextManager()->SetColor(textColor, "maxHealth");
	gui->GetTextManager()->SetPadding(textPadding, "maxHealth");
	gui->GetTextManager()->SetPos(textPos, "maxHealth");
	gui->GetTextManager()->SetScale(textScale, "maxHealth");
	gui->GetTextManager()->SetText(std::to_string(hc->GetMaxHealth()), "maxHealth");
	gui->GetTextManager()->SetBlend(textBlend, "maxHealth");

	float2 quadPos = { 0.3f, 0.85f };
	float2 quadScale = { 0.4f, 0.15f };
	float4 blended = { 1.0, 1.0, 1.0, 0.99 };
	float4 notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		"healthBackground",
		quadPos, quadScale,
		false, false,
		0,
		notBlended,
		healthBackgroundTexture);
#pragma endregion

#pragma region health holder
	entity = scene->AddEntity("healthHolder");
	gui = entity->AddComponent<component::GUI2DComponent>();
	quadPos = { 0.35f, 0.85f };
	quadScale = { 0.3f, 0.115f };
	gui->GetQuadManager()->CreateQuad(
		"healthHolder",
		quadPos, quadScale,
		false, false,
		1,
		notBlended,
		healthHolderTexture);
#pragma endregion

#pragma region health bar
	entity = scene->AddEntity("healthbar");
	gui = entity->AddComponent<component::GUI2DComponent>();
	quadPos = { 0.365f, 0.892f };
	quadScale = { 0.275f, 0.055f };
	gui->GetQuadManager()->CreateQuad(
		"healthbar",
		quadPos, quadScale,
		false, false,
		2,
		notBlended,
		healthbarTexture,
		float3{ 0.0f, 1.0f, 0.0f });
#pragma endregion

#pragma region health guardians
	entity = scene->AddEntity("healthGuardians");
	gui = entity->AddComponent<component::GUI2DComponent>();
	quadPos = { 0.32f, 0.86f };
	quadScale = { 0.3625f, 0.14f };
	gui->GetQuadManager()->CreateQuad(
		"healthGuardians",
		quadPos, quadScale,
		false, false,
		3,
		blended,
		healthGuardiansTexture);
#pragma endregion

#pragma region crosshair
	blended = { 1.0, 1.0, 1.0, 0.7 };
	entity = scene->AddEntity("crosshair");
	gui = entity->AddComponent<component::GUI2DComponent>();
	quadPos = { 0.497f, 0.495f };
	quadScale = { 0.006f, 0.01f };
	gui->GetQuadManager()->CreateQuad(
		"crosshair",
		quadPos, quadScale,
		false, false,
		3,
		blended,
		crosshairTexture);
#pragma endregion

#pragma region money
    textToRender = "0";
    textPos = { 0.95f, 0.03f };
    textPadding = { 0.5f, 0.0f };
    textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    textScale = { 0.4f, 0.4f };
    textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

    entity = scene->AddEntity("money");
    gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetTextManager()->SetFont(arial);
    gui->GetTextManager()->AddText("money");
    gui->GetTextManager()->SetColor(textColor, "money");
    gui->GetTextManager()->SetPadding(textPadding, "money");
    gui->GetTextManager()->SetPos(textPos, "money");
    gui->GetTextManager()->SetScale(textScale, "money");
    gui->GetTextManager()->SetText(textToRender, "money");
    gui->GetTextManager()->SetBlend(textBlend, "money");

    quadPos = { 0.91f, 0.03f };
    quadScale = { 0.03f, 0.03f };
    blended = { 1.0, 1.0, 1.0, 0.99 };
    notBlended = { 1.0, 1.0, 1.0, 1.0 };
    gui->GetQuadManager()->CreateQuad(
        "money",
        quadPos, quadScale,
        false, false,
        1,
        notBlended,
        currencyIcon
    );
#pragma endregion

#pragma region killed enemies
    textToRender = "0/20";
    textPos = { 0.074f, 0.044f };
    textPadding = { 0.5f, 0.0f };
    textColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    textScale = { 0.5f, 0.5f };
    textBlend = { 1.0f, 1.0f, 1.0f, 0.8f };

    entity = scene->AddEntity("enemyGui");
    gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetTextManager()->SetFont(arial);
    gui->GetTextManager()->AddText("enemyGui");
    gui->GetTextManager()->SetColor(textColor, "enemyGui");
    gui->GetTextManager()->SetPadding(textPadding, "enemyGui");
    gui->GetTextManager()->SetPos(textPos, "enemyGui");
    gui->GetTextManager()->SetScale(textScale, "enemyGui");
    gui->GetTextManager()->SetText(textToRender, "enemyGui");
    gui->GetTextManager()->SetBlend(textBlend, "enemyGui");

	quadPos = { 0.015f, 0.021f };
	quadScale = { 0.15f, 0.08f };
	blended = { 1.0, 1.0, 1.0, 0.9 };
	notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		"enemyGui",
		quadPos, quadScale,
		false, false,
		1,
		notBlended,
		killedEnemiesHolderTexture
	);
#pragma endregion
#pragma endregion

    scene->SetCollisionEntities(Physics::GetInstance().GetCollisionEntities());
    Physics::GetInstance().OnResetScene();

    scene->SetUpdateScene(&GameUpdateScene);

    scene->SetOnInit(&GameInitScene);

    return scene;
}

Scene* ShopScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("ShopScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::AnimationComponent* ac = nullptr;
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
    component::GUI2DComponent* gui = nullptr;
    component::CurrencyComponent* cur = nullptr;
    component::ParticleEmitterComponent* pec = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/IgnoredModels/Player/AnimatedPlayer.fbx");
    Model* shopModel = al->LoadModel(L"../Vendor/Resources/Models/Shop/shop.obj");
    Model* posterModel = al->LoadModel(L"../Vendor/Resources/Models/Poster/Poster.obj");
    Model* fenceModel = al->LoadModel(L"../Vendor/Resources/Models/FencePBR/fence.obj");
    Model* teleportModel = al->LoadModel(L"../Vendor/Resources/Models/Teleporter/Teleporter.obj");

	Texture* healthBackgroundTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HealthBackground.png");
	Texture* healthbarTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/Healthbar.png");
	Texture* healthGuardiansTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HealthGuardians.png");
	Texture* healthHolderTexture = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/HealthHolder.png");
    Texture* currencyIcon = al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/currency.png");

    TextureCubeMap* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");

	Font* arial = al->LoadFontFromFile(L"Arial.fnt");

#pragma region player
    std::string playerName = "player";
    Entity* entity = scene->AddEntity(playerName);
    mc = entity->AddComponent<component::ModelComponent>();
    ac = entity->AddComponent<component::AnimationComponent>();
    tc = entity->AddComponent<component::TransformComponent>(true);
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    mac = entity->AddComponent<component::MeleeComponent>();
    rc = entity->AddComponent<component::RangeComponent>(sm, scene, sphereModel, 0.3, 1, 20);
    uc = entity->AddComponent<component::UpgradeComponent>();
    cur = entity->AddComponent<component::CurrencyComponent>();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_ANIMATED | FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::NO_DEPTH);
    ac->Initialize();

    tc->GetTransform()->SetScale(0.05f);
    tc->GetTransform()->SetPosition(0.0, 1.0, 0.0);
    tc->SetTransformOriginalState();

    double3 playerDim = mc->GetModelDim();

    double rad = playerDim.z / 2.0;
    double cylHeight = playerDim.y - (rad * 2.0);
    ccc = entity->AddComponent<component::CapsuleCollisionComponent>(200.0, rad, cylHeight, 0.0, 0.0, false);
    hc = entity->AddComponent<component::HealthComponent>(50);
    ic->SetMovementSpeed(75.0);
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

    // Create test particleEffect
    ParticleEffectSettings settings = {};
    settings.maxParticleCount = 100;
    settings.startValues.lifetime = 0.8;
    settings.spawnInterval = settings.startValues.lifetime / settings.maxParticleCount;
    settings.startValues.acceleration = { 0, 0, 0 };

    // Need to fix EngineRand.rand() for negative values

    settings.randPosition = { -6, 6, 0, 15, -6, 6};
    settings.randVelocity = { -2, 2, 0, 2, -2, 2 };
    settings.randSize = { 0.3, 0.9 };
    settings.randRotationSpeed = { 0, 1 };

    Texture2DGUI* particleTexture = static_cast<Texture2DGUI*>(al->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/portal_particle_blue.png"));
    settings.texture = particleTexture;
    pec = entity->AddComponent<component::ParticleEmitterComponent>(&settings, true);
    
    mc->SetModel(teleportModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetPosition(-10.0f, 1.0f, -25.0f);
    tc->GetTransform()->SetScale(7.0f);
    tc->SetTransformOriginalState();
    
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);
    /* ---------------------- Teleporter ---------------------- */

	/* ------------------------- GUI --------------------------- */
	/* ----------------- healthBackground ---------------------- */
	std::string textToRender = "";
	float2 textPos = { 0.473f, 0.965f };
	float2 textPadding = { 0.8f, 0.0f };
	float4 textColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	float2 textScale = { 0.3f, 0.3f };
	float4 textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

	entity = scene->AddEntity("healthBackground");
	gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetTextManager()->SetFont(arial);
	gui->GetTextManager()->AddText("currentHealth");
	gui->GetTextManager()->SetColor(textColor, "currentHealth");
	gui->GetTextManager()->SetPadding(textPadding, "currentHealth");
	gui->GetTextManager()->SetPos(textPos, "currentHealth");
	gui->GetTextManager()->SetScale(textScale, "currentHealth");
	gui->GetTextManager()->SetText(std::to_string(hc->GetHealth()), "currentHealth");
	gui->GetTextManager()->SetBlend(textBlend, "currentHealth");

	textPos = { 0.499f, 0.965f };
	gui->GetTextManager()->AddText("slash");
	gui->GetTextManager()->SetColor(textColor, "slash");
	gui->GetTextManager()->SetPadding(textPadding, "slash");
	gui->GetTextManager()->SetPos(textPos, "slash");
	gui->GetTextManager()->SetScale(textScale, "slash");
	gui->GetTextManager()->SetText("/", "slash");
	gui->GetTextManager()->SetBlend(textBlend, "slash");

	textPos = { 0.503f, 0.965f };
	gui->GetTextManager()->AddText("maxHealth");
	gui->GetTextManager()->SetColor(textColor, "maxHealth");
	gui->GetTextManager()->SetPadding(textPadding, "maxHealth");
	gui->GetTextManager()->SetPos(textPos, "maxHealth");
	gui->GetTextManager()->SetScale(textScale, "maxHealth");
	gui->GetTextManager()->SetText(std::to_string(hc->GetMaxHealth()), "maxHealth");
	gui->GetTextManager()->SetBlend(textBlend, "maxHealth");

	float2 quadPos = { 0.3f, 0.85f };
	float2 quadScale = { 0.4f, 0.15f };
	float4 blended = { 1.0, 1.0, 1.0, 0.99 };
	float4 notBlended = { 1.0, 1.0, 1.0, 1.0 };
	gui->GetQuadManager()->CreateQuad(
		"healthBackground",
		quadPos, quadScale,
		false, false,
		0,
		notBlended,
		healthBackgroundTexture);
	/* ---------------------------------------------------------- */

	/* ------------------------- healthHolder --------------------------- */
	entity = scene->AddEntity("healthHolder");
	gui = entity->AddComponent<component::GUI2DComponent>();
	quadPos = { 0.35f, 0.85f };
	quadScale = { 0.3f, 0.115f };
	gui->GetQuadManager()->CreateQuad(
		"healthHolder",
		quadPos, quadScale,
		false, false,
		1,
		notBlended,
		healthHolderTexture);
	/* ---------------------------------------------------------- */

	/* ------------------------- healthbar --------------------------- */
	entity = scene->AddEntity("healthbar");
	gui = entity->AddComponent<component::GUI2DComponent>();
	quadPos = { 0.365f, 0.892f };
	quadScale = { 0.275f, 0.055f };
	gui->GetQuadManager()->CreateQuad(
		"healthbar",
		quadPos, quadScale,
		false, false,
		2,
		notBlended,
		healthbarTexture,
		float3{ 0.0f, 1.0f, 0.0f });
	/* ---------------------------------------------------------- */

	/* ------------------------- healthGuardians --------------------------- */
	entity = scene->AddEntity("healthGuardians");
	gui = entity->AddComponent<component::GUI2DComponent>();
	quadPos = { 0.32f, 0.86f };
	quadScale = { 0.3625f, 0.14f };
	gui->GetQuadManager()->CreateQuad(
		"healthGuardians",
		quadPos, quadScale,
		false, false,
		3,
		blended,
		healthGuardiansTexture);
	/* ---------------------------------------------------------- */

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

    /*---------------- GUI Coin -----------------*/
    textToRender = "0";
    textPos = { 0.95f, 0.03f };
    textPadding = { 0.5f, 0.0f };
    textColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    textScale = { 0.4f, 0.4f };
    textBlend = { 1.0f, 1.0f, 1.0f, 1.0f };

    entity = scene->AddEntity("money");
    gui = entity->AddComponent<component::GUI2DComponent>();
	gui->GetTextManager()->SetFont(arial);
    gui->GetTextManager()->AddText("money");
    gui->GetTextManager()->SetColor(textColor, "money");
    gui->GetTextManager()->SetPadding(textPadding, "money");
    gui->GetTextManager()->SetPos(textPos, "money");
    gui->GetTextManager()->SetScale(textScale, "money");
    gui->GetTextManager()->SetText(textToRender, "money");
    gui->GetTextManager()->SetBlend(textBlend, "money");

    quadPos = { 0.91f, 0.03f };
    quadScale = { 0.03f, 0.03f };
    notBlended = { 1.0, 1.0, 1.0, 1.0 };
    gui->GetQuadManager()->CreateQuad(
        "money",
        quadPos, quadScale,
        false, false,
        1,
        notBlended,
        currencyIcon
    );
    /*---------------- GUI -----------------*/
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

    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::PICKING);
    bbc->Init();
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

    /* ---------------------- moon ---------------------- */
    entity = scene->AddEntity("moon");
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::STATIC | FLAG_LIGHT::CAST_SHADOW);
    dlc->SetColor({ 0.8f, 0.8f, 0.8f });
    dlc->SetDirection({ 0.0f, -0.75f, 1.0f });
    dlc->SetCameraTop(50.0f);
    dlc->SetCameraBot(-30.0f);
    dlc->SetCameraLeft(-70.0f);
    dlc->SetCameraRight(70.0f);
    /* ---------------------- dirLight ---------------------- */

    scene->SetCollisionEntities(Physics::GetInstance().GetCollisionEntities());
    Physics::GetInstance().OnResetScene();

    /* ---------------------- Update Function ---------------------- */
    scene->SetUpdateScene(&ShopUpdateScene);

    scene->SetOnInit(&ShopInitScene);

    return scene;
}

void GameInitScene(Scene* scene)
{
    ParticleInit();

    scene->ResetNavMesh();

    std::vector<float3> spawnPoints;
    EnemyFactory* fact = &EnemyFactory::GetInstance();
    fact->ClearSpawnPoints();
    AssetLoader::Get()->GenerateMap(scene, "../Vendor/Resources/Rooms", &spawnPoints, { 3.0f,3.0f }, { 173.0f,200.0f }, true);

    for (int i = 0; i < spawnPoints.size(); i++)
    {
        fact->AddSpawnPoint(spawnPoints[i]);
    }

    AssetLoader::Get()->RemoveWalls();
}

void ShopInitScene(Scene* scene)
{
    ParticleInit();
}

void ParticleInit()
{
    ParticleEffectSettings settings = {};
    settings.maxParticleCount = 100;
    settings.startValues.lifetime = 1.5;
    settings.spawnInterval = 0.001;
    settings.startValues.acceleration = { 0, 0, 0 };
    settings.isLooping = false;

    // Need to fix EngineRand.rand() for negative values
    RandomParameter3 randParam1 = { -10, 10, -10, 10, -10, 10 };

    settings.randPosition = { -1, 1, -1, 1, -1, 1 };
    settings.randVelocity = randParam1;
    settings.randSize = { 0.2f, 0.6f };
    settings.randRotationSpeed = { 0, 3 };

    settings.texture = static_cast<Texture2DGUI*>(AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/portal_particle_blue.png"));
    Entity* particleEntity = SceneManager::GetInstance().GetActiveScene()->AddEntity("teleportationParticle");
    component::TransformComponent* transform = particleEntity->AddComponent<component::TransformComponent>();
    float3 position = Player::GetInstance().GetPlayer()->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
    transform->GetTransform()->SetPosition(position.x, position.y + 1, position.z);
    particleEntity->AddComponent<component::ParticleEmitterComponent>(&settings, true);
    particleEntity->GetComponent<component::ParticleEmitterComponent>()->OnInitScene();
    particleEntity->AddComponent<component::TemporaryLifeComponent>(1.5);
}

void GameUpdateScene(SceneManager* sm, double dt)
{
    if (ImGuiHandler::GetInstance().GetBool("reset"))
    {
        ImGuiHandler::GetInstance().SetBool("reset", false);
        EventBus::GetInstance().Publish(&ResetGame());
    }

    auto entities = Physics::GetInstance().GetCollisionEntities();
}

void ShopUpdateScene(SceneManager* sm, double dt)
{
    // Hidden Stefan & Hans
    static float rotValue = 0.0f;
    Transform* trans = sm->GetScene("ShopScene")->GetEntity("poster")->GetComponent<component::TransformComponent>()->GetTransform();
    trans->SetRotationX(rotValue);
    rotValue += 0.005f;

    // Kod-påkod-påkod-påkod-påkod-lösning
    // Detta ska egentligen stå i "OnShopGUIStateChange" i Shop, men eftersom att vi inte har samma
    // spelare i alla scener så kan vi ej nå den aktiva spelaren i den scenen därifrån.
    // TODO: Flytta in den i den funktionen när vi har samma spelare i alla scener via Player::GetInstance().
    if (Player::GetInstance().GetShop()->IsShop2DGUIDisplaying() == true)
    {
        component::CollisionComponent* cc = sm->GetActiveScene()->GetEntity("player")->GetComponent<component::CollisionComponent>();
        cc->SetVelVector(0.0f, 0.0f, 0.0f);
    }
}
