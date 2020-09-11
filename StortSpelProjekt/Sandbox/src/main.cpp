#include "Engine.h"

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

    /*------ AssetLoader to load models / textures ------*/
    AssetLoader* al = AssetLoader::Get();

    // This will be loaded once from disk, then the next time the same function is called (with the same filepath),
    // the function will just return the same pointer to the model that was loaded earlier.
    std::vector<Mesh*>* dragonModel = al->LoadModel(L"../Vendor/Resources/Models/Dragon/Dragon 2.5_fbx.fbx");
    
    // Needed for OBB test witten by björn. Should be removed after review
    std::vector<Mesh*>* stoneModel = al->LoadModel(L"../Vendor/Resources/Models/Rock/rock.obj");
    std::vector<Mesh*>* cubeModel = al->LoadModel(L"../Vendor/Resources/Models/Cube/crate.obj");

#pragma region CreateScene0
    // Create Scene
    Scene* scene = sceneManager->CreateScene("scene0");
    
    // Add Entity to Scene
    scene->AddEntity("player");
    scene->AddEntity("Dragon");
    scene->AddEntity("directionalLight");

    // Needed for OBB test witten by björn. Should be removed after review
    scene->AddEntity("box"); 
    scene->AddEntity("stone");

    // Add Components to Entities
    Entity* entity;

    entity = scene->GetEntity("player");
    component::CameraComponent* cc = entity->AddComponent<component::CameraComponent>(hInstance, *window->GetHwnd(), true);

    entity = scene->GetEntity("Dragon");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();

    entity = scene->GetEntity("directionalLight");
    entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_ULTRA_RESOLUTION);



    // Set the m_Components
    component::MeshComponent* mc = scene->GetEntity("Dragon")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(dragonModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    component::TransformComponent* tc = scene->GetEntity("Dragon")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 40.0f);
    tc->GetTransform()->RotateX(3.1415f / 2);

    // Needed for OBB test witten by björn. Should be removed after review
    entity = scene->GetEntity("box");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);

    entity = scene->GetEntity("stone");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(F_OBBFlags::COLLISION);


    Texture* ambientDefault = al->LoadTexture(L"../Vendor/Resources/Textures/Default/default_ambient.png");
    Texture* normalDefault  = al->LoadTexture(L"../Vendor/Resources/Textures/Default/default_normal.png");
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::AMBIENT , ambientDefault);
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::DIFFUSE , ambientDefault);
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::SPECULAR, ambientDefault);
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::NORMAL  , normalDefault);
    

    // Needed for OBB test witten by björn. Should be removed after review
    mc = scene->GetEntity("box")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc = scene->GetEntity("box")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-10.0f, 0.5f, 14.0f);
    scene->GetEntity("box")->GetComponent<component::BoundingBoxComponent>()->Init();
    // Needed for OBB test witten by björn. Should be removed after review
    mc = scene->GetEntity("stone")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(stoneModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    mc->GetMesh(0)->GetMaterial()->SetShininess(300);
    mc->GetMesh(0)->GetMaterial()->SetColorMul(COLOR_TYPE::LIGHT_SPECULAR, { 0.4f, 0.4f, 0.4f, 1.0f });
    tc = scene->GetEntity("stone")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.01f);
    tc->GetTransform()->SetPosition(-8.0f, 0.0f, 0.0f);
    scene->GetEntity("stone")->GetComponent<component::BoundingBoxComponent>()->Init();
    
    component::DirectionalLightComponent* dl = scene->GetEntity("directionalLight")->GetComponent<component::DirectionalLightComponent>();
    dl->SetDirection({ -1.0f, -1.0f, -1.0f });
    dl->SetColor(COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.05f, 0.05f, 1.0f });
    dl->SetColor(COLOR_TYPE::LIGHT_DIFFUSE, { 0.4f, 0.4f, 0.4f, 1.0f });
    dl->SetColor(COLOR_TYPE::LIGHT_SPECULAR, { 0.4f, 0.4f, 0.4f, 1.0f });


#pragma endregion CreateScene0

	char sceneName[10] = "scene0";
	sceneManager->SetSceneToDraw(sceneManager->GetScene(sceneName));
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
            Entity* pickedEnt = renderer->GetPickedEntity();
            if (pickedEnt != nullptr)
            {
				sceneManager->RemoveEntity(pickedEnt);
				scene->RemoveEntity(pickedEnt->GetName());
            }
		}
		if (window->WasSpacePressed())
		{
            
        }

        /* ------ Update ------ */
        timer->Update();
        renderer->Update(timer->GetDeltaTime());

        // Test for OBB collision written by Björn. Should  be removed after review
        tc = scene->GetEntity("stone")->GetComponent<component::TransformComponent>();
        static float test = 0;
        test += 0.0005;
        tc->GetTransform()->SetPosition(-8, 0, test);
        tc->GetTransform()->RotateX(test);
        Physics* p = engine.GetPhysics();
        if (p == nullptr)
        {
            p = new Physics();
        }
        //BoundingBoxComponent bbc = 
        //scene->GetEntity("stone")->GetComponent<component::BoundingBoxComponent>()->collision(scene->GetEntity("box")->GetComponent<component::BoundingBoxComponent>()->GetOBB(), *scene->GetEntity("box")->GetComponent<component::BoundingBoxComponent>()->GetTransform())
        if (p->checkOBBCollision(   
            scene->GetEntity("stone")->GetComponent<component::BoundingBoxComponent>()->GetOBB(),                   
            scene->GetEntity("box")->GetComponent<component::BoundingBoxComponent>()->GetOBB()))
        {
            Log::Print("Collision!\n");
        }
        else
        {
            //Log::Print("Nothing!\n");
        }
        DirectX::XMFLOAT3 corners[8];
        scene->GetEntity("stone")->GetComponent<component::BoundingBoxComponent>()->GetOBB().Center.x;
        Log::Print("stone corner z: %f..\n ",
            corners[2].z);
        delete p;

        /* ------ Sort ------ */
        renderer->SortObjectsByDistance();

        /* ------ Draw ------ */
        renderer->Execute();
    }

    return 0;
}
