#include "Engine.h"

Scene* JockesTestScene(SceneManager* sm);
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

    // This will be loaded once from disk, then the next time the same function is called (with the same filepath),
    // the function will just return the same pointer to the model that was loaded earlier.

    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/Floor/floor.obj");
    Model* panelModel = al->LoadModel(L"../Vendor/Resources/Models/Panel/panel.obj");
    Model* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    Model* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");
    Model* testModel = al->LoadModel(L"../Vendor/Resources/Models/test/dboy/D-boy2.obj");
    Model* playerModel = al->LoadModel(L"../Vendor/Resources/Models/Player/player.obj");
    Model* dragonModel = al->LoadModel(L"../Vendor/Resources/Models/Dragon/Dragon 2.5_fbx.fbx");

    //AUDIO EXAMPLE
    AudioBuffer* testAudio = al->LoadAudio(L"../Vendor/Resources/Audio/melody.wav", L"Melody");
    AudioBuffer* DABADABA = al->LoadAudio(L"../Vendor/Resources/Audio/AGameWithNoName.wav", L"Music");
    AudioBuffer* bruhAudio = al->LoadAudio(L"../Vendor/Resources/Audio/bruh.wav", L"Bruh");

    // To set an audio to loop, 0 = loop infinetly, 1 = loop once, x > 1 loop that amount of times.
    testAudio->SetAudioLoop(0);

#pragma region CreateScene0
    // Create Scene
    Scene* scene = sceneManager->CreateScene("scene0");

    // Add Entity to Scene
    scene->AddEntity("player");
    scene->AddEntity("floor");
    scene->AddEntity("box");
    scene->AddEntity("stone");
    scene->AddEntity("transparentTestObject");
    scene->AddEntity("Dragon");
    scene->AddEntity("directionalLight");
    scene->AddEntity("spotLight");
    scene->AddEntity("spotLight2");

    // Add Components to Entities
    Entity* entity;

    entity = scene->GetEntity("player");
    entity->AddComponent<component::ModelComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);
    entity->AddComponent<component::AudioVoiceComponent>();
    entity->AddComponent<component::Audio3DListenerComponent>();
    component::CameraComponent* cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true, CAMERA_FLAGS::USE_PLAYER_POSITION);

    entity = scene->GetEntity("floor");
    entity->AddComponent<component::ModelComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>();
    //entity->AddComponent<component::AudioVoiceComponent>();

    entity = scene->GetEntity("box");
    entity->AddComponent<component::ModelComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION | F_OBBFlags::PICKING);
    //entity->AddComponent<component::AudioVoiceComponent>();

    entity = scene->GetEntity("stone");
    entity->AddComponent<component::ModelComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION | F_OBBFlags::PICKING);
    // 3D sound test, adding a sound to the stone and then move around it for positional sound test
    entity->AddComponent<component::Audio3DEmitterComponent>();

    entity = scene->GetEntity("transparentTestObject");
    entity->AddComponent<component::ModelComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>();

    entity = scene->GetEntity("Dragon");
    entity->AddComponent<component::ModelComponent>();
    entity->AddComponent<component::TransformComponent>();

    entity = scene->GetEntity("directionalLight");
    entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION);

    entity = scene->GetEntity("spotLight");
    entity->AddComponent<component::ModelComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>();
    entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION | FLAG_LIGHT::USE_TRANSFORM_POSITION);

    // Add entities with collision enabled to the vector used for collision checking in Physics
    physics->AddCollisionEntity(scene->GetEntity("stone"));
    physics->AddCollisionEntity(scene->GetEntity("player"));
    physics->AddCollisionEntity(scene->GetEntity("box"));


    // Set the m_Components

    component::ModelComponent* mc = scene->GetEntity("player")->GetComponent<component::ModelComponent>();
    mc->SetModel(playerModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    component::TransformComponent* tc = scene->GetEntity("player")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, 0);
    scene->GetEntity("player")->GetComponent<component::BoundingBoxComponent>()->Init();

    // To add sound for an entity, a voice component needs to be added. Add the voice component with the same name as
    // one of the loaded audios. The same audio may be played from different entities!
    //component::AudioVoiceComponent* avc = scene->GetEntity("player")->GetComponent<component::AudioVoiceComponent>();
    //avc->AddVoice(L"Bruh");

    // To play the audio, simply call play on the component, giving the name of the Audio that you whish to play.
    // Multiple sounds are allowed!
    //avc->Play(L"Bruh");


    mc = scene->GetEntity("floor")->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc = scene->GetEntity("floor")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(20, 1, 20);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    scene->GetEntity("floor")->GetComponent<component::BoundingBoxComponent>()->Init();

    mc = scene->GetEntity("box")->GetComponent<component::ModelComponent>();
    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc = scene->GetEntity("box")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-10.0f, 0.5f, 14.0f);
    scene->GetEntity("box")->GetComponent<component::BoundingBoxComponent>()->Init();

    mc = scene->GetEntity("stone")->GetComponent<component::ModelComponent>();
    mc->SetModel(stoneModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc = scene->GetEntity("stone")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.01f);
    tc->GetTransform()->SetPosition(-8.0f, 0.0f, 0.0f);
    scene->GetEntity("stone")->GetComponent<component::BoundingBoxComponent>()->Init();

    // Audio, add sound to stone for 3d testing
    component::Audio3DEmitterComponent* audioEmitter = scene->GetEntity("stone")->GetComponent<component::Audio3DEmitterComponent>();
    audioEmitter->AddVoice(L"Melody");
    audioEmitter->Play(L"Melody");

    mc = scene->GetEntity("transparentTestObject")->GetComponent<component::ModelComponent>();
    mc->SetModel(panelModel);
    mc->SetDrawFlag(FLAG_DRAW::Blend);

    mc = scene->GetEntity("Dragon")->GetComponent<component::ModelComponent>();
    mc->SetModel(dragonModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc = scene->GetEntity("Dragon")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 40.0f);
    tc->GetTransform()->SetRotationX(3.1415f / 2);


    Texture* ambientDefault = al->LoadTexture(L"../Vendor/Resources/Textures/Default/default_ambient.png");
    Texture* normalDefault = al->LoadTexture(L"../Vendor/Resources/Textures/Default/default_normal.png");

    (*mc->GetTexturesAt(0))[TEXTURE_TYPE::AMBIENT] = ambientDefault;
    (*mc->GetTexturesAt(0))[TEXTURE_TYPE::DIFFUSE] = ambientDefault;
    (*mc->GetTexturesAt(0))[TEXTURE_TYPE::SPECULAR] = ambientDefault;
    (*mc->GetTexturesAt(0))[TEXTURE_TYPE::NORMAL] = ambientDefault;

    tc = scene->GetEntity("transparentTestObject")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(5.0f);
    tc->GetTransform()->SetPosition(0.0f, 5.0f, 1.0f);
    tc->GetTransform()->SetRotationZ(3.141572f / 2.0f);
    tc->GetTransform()->SetRotationX(3.141572f / 2.0f);

    entity = scene->GetEntity("transparentTestObject");
    entity->GetComponent<component::BoundingBoxComponent>()->Init();

    component::DirectionalLightComponent* dl = scene->GetEntity("directionalLight")->GetComponent<component::DirectionalLightComponent>();
    dl->SetDirection({ -1.0f, -1.0f, -1.0f });
    dl->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.05f, 0.05f, 1.0f });
    dl->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.4, 0.4, 0.4, 1.0f });
    dl->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.4f, 0.4f, 0.4f, 1.0f });

    // Spotlight settings
    entity = scene->GetEntity("spotLight");
    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering);

    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.3f);
    tc->GetTransform()->SetPosition(-20.0f, 6.0f, -3.0f);

    entity->GetComponent<component::BoundingBoxComponent>()->Init();

    component::SpotLightComponent* sl = scene->GetEntity("spotLight")->GetComponent<component::SpotLightComponent>();
    sl->SetPosition({ -20.0f, 6.0f, -3.0f });
    sl->SetDirection({ 2.0, -1.0, 0.0f });
    sl->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.00f, 0.05f, 1.0f });
    sl->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.9f, 0.00f, 0.9f, 1.0f });
    sl->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.9f, 0.00f, 0.9f, 1.0f });

#pragma endregion CreateScene0

    char sceneName[10] = "scene0";
    //scene = JockesTestScene(sceneManager);
	//scene = FredriksTestScene(sceneManager);

    //scene = sceneManager->GetScene(sceneName);
    sceneManager->SetSceneToDraw(scene);

    while (!window->ExitWindow())
    {
        // ONLY HERE FOR TESTING
        if (window->WasTabPressed())
        {
            // Test to change scene during runtime
            //static int sceneSwapper = 1;
            //sceneSwapper %= 2;
            //sprintf(sceneName, "scene%d", sceneSwapper);
            //Log::Print("Scene: %s\n", sceneName);
            //sceneManager->SetSceneToDraw(sceneManager->GetScene(sceneName));
            //sceneSwapper++;

            // Test to remove picked object
            /*Entity* pickedEnt = renderer->GetPickedEntity();
            if (pickedEnt != nullptr)
            {
                sceneManager->RemoveEntity(pickedEnt);
                scene->RemoveEntity(pickedEnt->GetName());
            }*/
		}

		if (window->WasSpacePressed())
		{
        }

        
        // 3d sound test
        component::Audio3DEmitterComponent* emitterTest = scene->GetEntity("stone")->GetComponent<component::Audio3DEmitterComponent>();
        emitterTest->UpdatePosition();
        component::Audio3DListenerComponent* listenerTest = scene->GetEntity("player")->GetComponent<component::Audio3DListenerComponent>();
        listenerTest->UpdatePosition();



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
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true, CAMERA_FLAGS::USE_PLAYER_POSITION);

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

    plc->SetAttenuation({1.0f, 0.045f, 0.0075});
    
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
	cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true, CAMERA_FLAGS::USE_PLAYER_POSITION);

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
	textComp->AddText();
	textComp->SetColor(textColor, 0);
	textComp->SetPadding(textPadding, 0);
	textComp->SetPos(textPos, 0);
	textComp->SetScale(textScale, 0);
	textComp->SetText(textToRender, 0);

	/* ---------------------------------------------------------- */

	return scene;
}
