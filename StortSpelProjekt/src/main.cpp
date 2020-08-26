#include "Engine/Engine.h"

// Helps intellisense to understand that stdafx is included
#include "Engine/Headers/stdafx.h"

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
	SceneHandler* const sceneHandler = engine.GetSceneHandler();
	Renderer* const renderer = engine.GetRenderer();

    // This will be loaded once from disk, then the next time the same function is called (with the same filepath),
    // the function will just return the same pointer to the model that was loaded earlier.
    std::vector<Mesh*>* floorModel = renderer->LoadModel(L"../Resources/Models/Floor/floor.obj");
    std::vector<Mesh*>* stoneModel = renderer->LoadModel(L"../Resources/Models/Rock/rock.obj");
    std::vector<Mesh*>* cubeModel  = renderer->LoadModel(L"../Resources/Models/Cube/crate.obj");

#pragma region CreateScene0
    // Create Scene
    Scene* scene = sceneHandler->CreateScene("scene0");
    
    // Add Entity to Scene
    scene->AddEntity("player");
    scene->AddEntity("floor");
    scene->AddEntity("box");
    scene->AddEntity("stone");
    scene->AddEntity("transparentTestObject");
    scene->AddEntity("directionalLight");
    scene->AddEntity("spotLight");
    scene->AddEntity("spotLight2");

    // Add Components to Entities
    Entity* entity;

    entity = scene->GetEntity("player");
    component::CameraComponent* cc = entity->AddComponent<component::CameraComponent>(hInstance, *window->GetHwnd(), true);

    entity = scene->GetEntity("floor");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(false);

    entity = scene->GetEntity("box");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(true);

    entity = scene->GetEntity("stone");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(true);

    entity = scene->GetEntity("transparentTestObject");
    entity->AddComponent<component::MeshComponent>();
    entity->AddComponent<component::TransformComponent>();
    entity->AddComponent<component::BoundingBoxComponent>(false);

    entity = scene->GetEntity("directionalLight");
    entity->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_LOW_RESOLUTION);

    entity = scene->GetEntity("spotLight");
    entity->AddComponent<component::SpotLightComponent>(FLAG_LIGHT::CAST_SHADOW_LOW_RESOLUTION);


    // Set the components
    component::MeshComponent* mc = scene->GetEntity("floor")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    component::TransformComponent* tc = scene->GetEntity("floor")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(20, 1, 20);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    scene->GetEntity("floor")->GetComponent<component::BoundingBoxComponent>()->Init();

    mc = scene->GetEntity("box")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc = scene->GetEntity("box")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-10.0f, 0.5f, 14.0f);
    scene->GetEntity("box")->GetComponent<component::BoundingBoxComponent>()->Init();

    mc = scene->GetEntity("stone")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(stoneModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    tc = scene->GetEntity("stone")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.01f);
    tc->GetTransform()->SetPosition(-8.0f, 0.0f, 0.0f);
    scene->GetEntity("stone")->GetComponent<component::BoundingBoxComponent>()->Init();

    mc = scene->GetEntity("transparentTestObject")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::Blend);

    Texture* ambientDefault = renderer->LoadTexture(L"../Resources/Textures/Default/default_ambient.png");
    Texture* normalDefault = renderer->LoadTexture(L"../Resources/Textures/Default/default_normal.png");
    mc->GetMesh(0)->SetTexture(TEXTURE_TYPE::AMBIENT , ambientDefault);
    mc->GetMesh(0)->SetTexture(TEXTURE_TYPE::DIFFUSE , ambientDefault);
    mc->GetMesh(0)->SetTexture(TEXTURE_TYPE::SPECULAR, ambientDefault);
    mc->GetMesh(0)->SetTexture(TEXTURE_TYPE::NORMAL  , normalDefault);
    
    tc = scene->GetEntity("transparentTestObject")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(5.0f);
    tc->GetTransform()->SetPosition(0.0f, 5.0f, 1.0f);
    tc->GetTransform()->RotateZ(3.141572f / 2.0f);
    tc->GetTransform()->RotateX(3.141572f / 2.0f);
    
    entity = scene->GetEntity("transparentTestObject");
    entity->GetComponent<component::BoundingBoxComponent>()->Init();

    component::DirectionalLightComponent* dl = scene->GetEntity("directionalLight")->GetComponent<component::DirectionalLightComponent>();
    dl->SetDirection({ -1.0f, -1.0f, -1.0f });
    dl->SetColor(LIGHT_COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.05f, 0.05f, 1.0f });
    dl->SetColor(LIGHT_COLOR_TYPE::LIGHT_DIFFUSE, { 0.4f, 0.4f, 0.4f, 1.0f });
    dl->SetColor(LIGHT_COLOR_TYPE::LIGHT_SPECULAR, { 0.4f, 0.4f, 0.4f, 1.0f });

    component::SpotLightComponent* sl = scene->GetEntity("spotLight")->GetComponent<component::SpotLightComponent>();
    sl->SetPosition({ -20.0f, 6.0f, -3.0f });
    sl->SetDirection({ 2.0, -1.0, 0.0f });
    sl->SetColor(LIGHT_COLOR_TYPE::LIGHT_AMBIENT, { 0.05f, 0.00f, 0.05f, 1.0f });
    sl->SetColor(LIGHT_COLOR_TYPE::LIGHT_DIFFUSE, { 1.0f, 0.00f, 1.0f, 1.0f });
    sl->SetColor(LIGHT_COLOR_TYPE::LIGHT_SPECULAR, { 1.0f, 0.00f, 1.0f, 1.0f });

#pragma endregion CreateScene0
#pragma region CreateScene1
    // Create Scene
    sceneHandler->CreateScene("scene1");
    Scene* scene1 = sceneHandler->GetScene("scene1");

    // Use the same player as in the first scene
    entity = scene->GetEntity("player");
    scene1->AddEntityFromOther(entity);
    
    scene1->AddEntity("cube1");
    scene1->AddEntity("cube2");
    scene1->AddEntity("directionalLight");
    scene1->GetEntity("cube1")->AddComponent<component::MeshComponent>();
    scene1->GetEntity("cube1")->AddComponent<component::TransformComponent>();
    scene1->GetEntity("cube2")->AddComponent<component::MeshComponent>();
    scene1->GetEntity("cube2")->AddComponent<component::TransformComponent>();
    scene1->GetEntity("directionalLight")->AddComponent<component::DirectionalLightComponent>(FLAG_LIGHT::CAST_SHADOW_LOW_RESOLUTION);
    
    mc = scene1->GetEntity("cube1")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    
    tc = scene1->GetEntity("cube1")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-15.0f, 1.0f, 0.0f);
    
    mc = scene1->GetEntity("cube2")->GetComponent<component::MeshComponent>();
    mc->SetMeshes(cubeModel);
    mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
    
    tc = scene1->GetEntity("cube2")->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(0.5f);
    tc->GetTransform()->SetPosition(-5.0f, 1.0f, 0.0f);
   
    dl = scene1->GetEntity("directionalLight")->GetComponent<component::DirectionalLightComponent>();
    dl->SetDirection({ -1.0f, -1.0f, -1.0f });
    dl->SetColor(LIGHT_COLOR_TYPE::LIGHT_AMBIENT, { 0.02f, 0.08f, 0.08f, 1.0f });
    dl->SetColor(LIGHT_COLOR_TYPE::LIGHT_DIFFUSE, { 0.2f, 0.8f, 0.8f, 1.0f });
    dl->SetColor(LIGHT_COLOR_TYPE::LIGHT_SPECULAR, { 0.2f, 0.8f, 0.8f, 1.0f });

#pragma endregion CreateScene1
    renderer->SetSceneToDraw(sceneHandler->GetScene("scene0"));
	sceneHandler->ManageScene(sceneHandler->GetScene("scene0"));
    while (!window->ExitWindow())
    {
        // ONLY HERE FOR TESTING
        if (window->WasSpacePressed())
        {
            // Test to change scene during runtime
            //char sceneName[10];
            //static int sceneSwapper = 1;
            //sceneSwapper %= 2;
            //sprintf(sceneName, "scene%d", sceneSwapper);
            //renderer.SetSceneToDraw(sceneHandler->GetScene(sceneName));
            //sceneSwapper++;

            // Test to move objects during runtime
            //scene = sceneHandler->GetScene("scene0");
            //tc = scene->GetEntity("stone")->GetComponent<component::TransformComponent>();
            //float3 posa = tc->GetTransform()->GetPositionFloat3();
            //tc->GetTransform()->SetPosition(posa.x, posa.y, posa.z + 0.1f);

            // Test to add objects during runtime (horrible solution, very badly designed)
            char boxName[10];
            static int boxisCounter = 1;
            sprintf(boxName, "boxis%d", boxisCounter);
            boxisCounter++;
            
            scene = sceneHandler->GetScene("scene0");
            entity = scene->AddEntity(boxName);
            entity->AddComponent<component::MeshComponent>();
            entity->AddComponent<component::TransformComponent>();
            
            mc = entity->GetComponent<component::MeshComponent>();
            mc->SetMeshes(cubeModel);
            mc->SetDrawFlag(FLAG_DRAW::ForwardRendering | FLAG_DRAW::Shadow);
            
            tc = entity->GetComponent<component::TransformComponent>();
            tc->GetTransform()->SetScale(0.5f);
            float3 spawnPosition = { cc->GetCamera()->GetPositionFloat3().x + cc->GetCamera()->GetLookAt().x * 10,
                                     cc->GetCamera()->GetPositionFloat3().y + cc->GetCamera()->GetLookAt().y * 10, 
                                     cc->GetCamera()->GetPositionFloat3().z + cc->GetCamera()->GetLookAt().z * 10, };
            tc->GetTransform()->SetPosition(spawnPosition.x, spawnPosition.y, spawnPosition.z);
            
            // (horrible solution, very badly designed)
            renderer->SetSceneToDraw(sceneHandler->GetScene("scene0"));
			sceneHandler->ManageScene(entity);
        }

        /* ------ Update ------ */
        timer->Update();
        renderer->Update(timer->GetDeltaTime());

        /* ------ Sort ------ */
        renderer->SortObjectsByDistance();
        /* ------ Draw ------ */
        renderer->Execute();
    }

    return 0;
}
