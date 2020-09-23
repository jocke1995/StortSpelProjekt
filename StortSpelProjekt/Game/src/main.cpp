#include "Engine.h"
#include "Components/PlayerInputComponent.h"
Scene* GetDemoScene(SceneManager* sm);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /* ------ Engine  ------ */
    Engine engine;
    engine.Init(hInstance, nCmdShow);

    /*------ Load Option Variables ------*/
    Option::GetInstance().ReadFile();
    float updateRate = 1.0f / Option::GetInstance().GetVariable("updateRate");

	/*  ------ Get references from engine  ------ */
	Window* const window = engine.GetWindow();
	Timer* const timer = engine.GetTimer();
	ThreadPool* const threadPool = engine.GetThreadPool();
	SceneManager* const sceneManager = engine.GetSceneHandler();
	Renderer* const renderer = engine.GetRenderer();

    sceneManager->SetSceneToDraw(GetDemoScene(sceneManager));

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
                
                Physics::GetInstance().Update(updateRate);
                renderer->Update(updateRate);
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
    Scene* scene = sm->CreateScene("DevScene");


    /*--------------------- Assets ---------------------*/

    AssetLoader* al = AssetLoader::Get();

    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/cube.obj");


    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    /*--------------------- Assets ---------------------*/

    /*--------------------- Component declarations ---------------------*/
    Entity* entity = nullptr;
    component::AudioVoiceComponent* avc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::PlayerInputComponent* pic = nullptr;
    component::TextComponent* txc = nullptr;
    /*--------------------- Component declarations ---------------------*/

    /*--------------------- Player ---------------------*/
    // entity
    entity = scene->AddEntity("player");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    pic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::AudioVoiceComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    pic->Init();

    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0.0f, 1.0f, -30.0f);

    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);

    avc->AddVoice(L"Bruh");
    /*--------------------- Player ---------------------*/

    /* ---------------------- Skybox ---------------------- */

    // Skybox
    Texture* skyboxCubemap = al->LoadTextureCubeMap(L"../Vendor/Resources/Textures/CubeMaps/skymap.dds");
    entity = scene->AddEntity("skybox");
    component::SkyboxComponent* sbc = entity->AddComponent<component::SkyboxComponent>();
    sbc->SetMesh(cubeModel->GetMeshAt(0));
    sbc->SetTexture(skyboxCubemap);

    sbc->SetCamera(cc->GetCamera());
    sbc->GetTransform()->SetScale(1);

    /* ---------------------- Skybox ---------------------- */


    /*--------------------- Rock ---------------------*/
    // entity
    entity = scene->AddEntity("rock");
    
    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    bbc = entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION | F_OBBFlags::PICKING);
    
    
    mc->SetModel(rockModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(0.01f);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    
    bbc->Init();
    Physics::GetInstance().AddCollisionEntity(entity);
    /*--------------------- Rock ---------------------*/

    /*--------------------- Floor ---------------------*/
    // entity
    entity = scene->AddEntity("floor");
    
    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    
    
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::GIVE_SHADOW | FLAG_DRAW::DRAW_OPAQUE);
    tc->GetTransform()->SetScale(35.0f, 1.0f, 35.0f);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /*--------------------- Floor ---------------------*/

    /*--------------------- PointLight 1 ---------------------*/
    // entity
    entity = scene->AddEntity("pointLightA");
    
    // components
    plc = entity->AddComponent<component::PointLightComponent>();
    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.5f, 0.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 5.0f, 0.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.9f, 0.0f, 1.0f });
    plc->SetPosition({ -30,1,30 });
    /*--------------------- PointLight 1 ---------------------*/
    
    /*--------------------- PointLight 2 ---------------------*/
    // entity
    entity = scene->AddEntity("pointLightB");
    
    // components
    plc = entity->AddComponent<component::PointLightComponent>();
    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.5f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 5.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.9f, 0.0f, 1.0f });
    plc->SetPosition({ 30,1,30 });
    /*--------------------- PointLight 2 ---------------------*/
    
    /*--------------------- PointLight 3 ---------------------*/
    // entity
    entity = scene->AddEntity("pointLightC");
    
    // components
    plc = entity->AddComponent<component::PointLightComponent>();
    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.0f, 0.5f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.0f, 0.0f, 10.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.0f, 9.0f, 1.0f });
    plc->SetPosition({ 30,1,-30 });
    /*--------------------- PointLight 3 ---------------------*/

    /*--------------------- PointLight 4 ---------------------*/
    // entity
    entity = scene->AddEntity("pointLightD");

    // components
    plc = entity->AddComponent<component::PointLightComponent>();
    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.5f, 0.5f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 5.0f, 5.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.9f, 0.0f, 1.0f });
    plc->SetPosition({ -30,1,-30 });
    /*--------------------- PointLight 4 ---------------------*/

    /*--------------------- DirectionalLight ---------------------*/
    // entity
    entity = scene->AddEntity("sun");
    
    // components
    dlc = entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION);
    dlc->SetDirection({1.0f, -1.0f, -1.0f});
    dlc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.1f, 0.1f, 0.1f, 1.0f });
    dlc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.1f, 0.1f, 0.1f, 1.0f });
    dlc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.5f, 0.5f, 0.5f, 1.0f });
    /*--------------------- DirectionalLight ---------------------*/

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