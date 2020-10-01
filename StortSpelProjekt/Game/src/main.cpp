#include "Engine.h"
#include "Components/PlayerInputComponent.h"
#include "Components/HealthComponent.h"
#include "EnemyFactory.h"

Scene* GetDemoScene(SceneManager* sm);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	/*------ Load Option Variables ------*/
	Option::GetInstance().ReadFile();
	float updateRate = 1.0f / std::atof(Option::GetInstance().GetVariable("f_updateRate").c_str());

    /* ------ Engine  ------ */
    Engine engine;
    engine.Init(hInstance, nCmdShow);

	/*  ------ Get references from engine  ------ */
	Window* const window = engine.GetWindow();
	Timer* const timer = engine.GetTimer();
	ThreadPool* const threadPool = engine.GetThreadPool();
	SceneManager* const sceneManager = engine.GetSceneHandler();
	Renderer* const renderer = engine.GetRenderer();
	
    sceneManager->SetScene(GetDemoScene(sceneManager));

    double logicTimer = 0;

    if (renderer->GetActiveScene())
    {
        while (!window->ExitWindow())
        {
            // Currently no scene set, hence the m_pRenderer should not be working.

            /* ------ Update ------ */
            timer->Update();
            logicTimer += timer->GetDeltaTime();

            renderer->RenderUpdate(timer->GetDeltaTime());
            if (logicTimer >= updateRate)
            {
                logicTimer = 0;
                
                renderer->Update(updateRate);
                Physics::GetInstance().Update(updateRate);
            }

            /* ------ Sort ------ */
            renderer->SortObjects();

            /* ------ Draw ------ */
            renderer->Execute();
        }
    }

    return 0;
}

Scene* GetDemoScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("devScene");


    /*--------------------- Assets ---------------------*/

    AssetLoader* al = AssetLoader::Get();

    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    //Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    //Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* barbModel = al->LoadModel(L"../Vendor/Resources/Models/Barb/conan_obj.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    /*--------------------- Assets ---------------------*/

    /*--------------------- Component declarations ---------------------*/
    Entity* entity = nullptr;
    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::InputComponent* ic = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    component::CollisionComponent* bcc = nullptr;


    /* ---------------------- Player ---------------------- */
    entity = scene->AddEntity("player");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::SphereCollisionComponent>(1, 1.5,0.1);
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    ic->Init();
    // adding OBB with collision
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    entity->AddComponent<component::HealthComponent>(10);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    avc->AddVoice(L"Bruh");

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 2, -25);
    // initialize OBB after we have the transform info
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);
    /* ---------------------- Player ---------------------- */

    /* ---------------------- Skybox ---------------------- */

    // Skybox
    TextureCubeMap* skyboxCubeMap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");
    entity = scene->AddEntity("skybox");

    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    /* ---------------------- Skybox ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bcc = entity->AddComponent<component::CubeCollisionComponent>(0.0, 35,0,35);
    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0, 0.0, 0.0);
    /* ---------------------- Floor ---------------------- */

    /*--------------------- Adding 76 Enemies for preformance check ---------------------*/

    EnemyFactory enH(scene);
    enH.AddEnemy("barb", barbModel, 5, float3{ 1, 0, 1 }, F_COMP_FLAGS::OBB, 0.3, float3{ 0, 0, 0 });

    // looping through and adding already existing enemy type with only new position
    float xVal = 8;
    float zVal = 0;
    for (int i = 0; i < 75; i++)
    {
        zVal += 8;
        enH.AddExistingEnemy("barb", float3{ xVal - 64, 0, zVal });
        if ((i + 1) % 5 == 0)
        {
            xVal += 8;
            zVal = 0;
        }
    }
    /*--------------------- Adding 76 Enemies for preformance check ---------------------*/

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

	///*--------------------- Text ---------------------*/
	//
	//// font
	//std::pair<Font*, Texture*> arialFont = al->LoadFontFromFile(L"Arial.fnt");
	//
	//// text properties
	//std::string textToRender = "Daedalus Maze 2:\nThe Return of the Minotaur";
	//float2 textPos = { 0.02f, 0.01f };
	//float2 textPadding = { 0.5f, 0.0f };
	//float4 textColor = { 1.0f, 0.2f, 1.0f, 1.0f };
	//float2 textScale = { 0.5f, 0.5f };
	//
	//// entity
	//entity = scene->AddEntity("textbox");
	//
	////component
	//txc = entity->AddComponent<component::TextComponent>(arialFont);
	//
	//txc->AddText("text");
	//txc->SetColor(textColor, "text");
	//txc->SetPadding(textPadding, "text");
	//txc->SetPos(textPos, "text");
	//txc->SetScale(textScale, "text");
	//txc->SetText(textToRender, "text");
	///*--------------------- Text ---------------------*/

    return scene;
}