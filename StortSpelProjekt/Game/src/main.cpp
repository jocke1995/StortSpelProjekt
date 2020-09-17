#include "Engine.h"

Scene* GetDevScene(SceneManager* sm);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /* ------ Engine  ------ */
    Engine engine;
    engine.Init(hInstance, nCmdShow);

	/*  ------ Get references from engine  ------ */
	Window* const window = engine.GetWindow();
	Timer* const timer = engine.GetTimer();
	ThreadPool* const threadPool = engine.GetThreadPool();
	SceneManager* const sceneManager = engine.GetSceneHandler();
	Renderer* const renderer = engine.GetRenderer();

    sceneManager->SetSceneToDraw(GetDevScene(sceneManager));

    if (renderer->GetActiveScene())
    {
        while (!window->ExitWindow())
        {
            // Currently no scene set, hence the m_pRenderer should not be working.

            /* ------ Update ------ */
            timer->Update();
            renderer->Update(timer->GetDeltaTime());

            /* ------ Sort ------ */
            renderer->SortObjectsByDistance();

            /* ------ Draw ------ */
            renderer->Execute();
        }
    }

    return 0;
}

Scene* GetDevScene(SceneManager* sm)
{
    Scene* scene = sm->CreateScene("DevScene");


    /*--------------------- Assets ---------------------*/
    
    AssetLoader* al = AssetLoader::Get();
    
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* rockModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");

    AudioBuffer* bruhVoice = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");
    /*--------------------- Assets ---------------------*/

    /*--------------------- Component declarations ---------------------*/
    Entity* entity = nullptr;
    component::AudioVoiceComponent* avc = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::TransformComponent* tc = nullptr;
    /*--------------------- Component declarations ---------------------*/

    /*--------------------- Player ---------------------*/
    // entity
    entity = scene->AddEntity("player");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true, CAMERA_FLAGS::USE_PLAYER_POSITION);
    avc = entity->AddComponent<component::AudioVoiceComponent>();


    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0.0f, 1.0f, -30.0f);
    avc->AddVoice(L"Bruh");
    /*--------------------- Player ---------------------*/


    /*--------------------- Rock ---------------------*/
    // entity
    entity = scene->AddEntity("rock");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();


    mc->SetModel(rockModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc->GetTransform()->SetScale(0.01f);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /*--------------------- Rock ---------------------*/

    /*--------------------- Floor ---------------------*/
    // entity
    entity = scene->AddEntity("floor");

    // components
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();


    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc->GetTransform()->SetScale(35.0f, 1.0f, 35.0f);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /*--------------------- Floor ---------------------*/

    /*--------------------- Light ---------------------*/
    // entity
    entity = scene->AddEntity("pointLight");

    // components
    plc = entity->AddComponent<component::PointLightComponent>();
    plc->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.0f, 0.0f, 5.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 10.0f, 10.0f, 0.0f, 1.0f });
    plc->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.9f, 0.0f, 1.0f });
    /*--------------------- Light ---------------------*/

    return scene;
}