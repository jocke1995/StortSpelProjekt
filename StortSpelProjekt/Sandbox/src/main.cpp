#include "Engine.h"
#include "TestHeader.h"
#include "TestHeaderTwo.h"

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

    TestSubscriber testingOne;
    TestHeaderTwo testingTwo;

    EventBus::get().publish(&MessageLog("Message"));

#pragma region CreateScene0
    // Create Scene
    Scene* scene = sceneManager->CreateScene("scene0");
    
    // Add Entity to Scene
    scene->AddEntity("player");
    scene->AddEntity("Dragon");
    scene->AddEntity("directionalLight");

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


    Texture* ambientDefault = al->LoadTexture(L"../Vendor/Resources/Textures/Default/default_ambient.png");
    Texture* normalDefault  = al->LoadTexture(L"../Vendor/Resources/Textures/Default/default_normal.png");
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::AMBIENT , ambientDefault);
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::DIFFUSE , ambientDefault);
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::SPECULAR, ambientDefault);
    mc->GetMesh(0)->GetMaterial()->SetTexture(TEXTURE_TYPE::NORMAL  , normalDefault);
    
    
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

        /* ------ Sort ------ */
        renderer->SortObjectsByDistance();

        /* ------ Draw ------ */
        renderer->Execute();
    }

    return 0;
}
