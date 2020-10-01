#include "Engine.h"
#include "Components/PlayerInputComponent.h"
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
	
    sceneManager->SetSceneToDraw(GetDemoScene(sceneManager));

    double logicTimer = 0;

    if (renderer->GetActiveScene())
    {
        while (!window->ExitWindow())
        {
			while (g_ProgramPaused == false)
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

				g_ProgramStarted = true;

				/* ------ Draw ------ */
				renderer->Execute();
			}
        }
    }

    return 0;
}

Scene* GetDemoScene(SceneManager* sm)
{
    // Create Scene
    Scene* scene = sm->CreateScene("devScene");

    component::CameraComponent* cc = nullptr;
    component::ModelComponent* mc = nullptr;
    component::TransformComponent* tc = nullptr;
    component::InputComponent* ic = nullptr;
    component::BoundingBoxComponent* bbc = nullptr;
    component::PointLightComponent* plc = nullptr;
    component::DirectionalLightComponent* dlc = nullptr;
    component::SpotLightComponent* slc = nullptr;
    component::Audio2DVoiceComponent* avc = nullptr;
    AssetLoader* al = AssetLoader::Get();

    // Get the models needed
    Model* floorModel = al->LoadModel(L"../Vendor/Resources/Models/FloorPBR/floor.obj");
    Model* sphereModel = al->LoadModel(L"../Vendor/Resources/Models/SpherePBR/ball.obj");

    /* ---------------------- Player ---------------------- */
    Entity* entity = (scene->AddEntity("player"));
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();
    ic = entity->AddComponent<component::PlayerInputComponent>(CAMERA_FLAGS::USE_PLAYER_POSITION);
    cc = entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    avc = entity->AddComponent<component::Audio2DVoiceComponent>();
    ic->Init();

    mc->SetModel(sphereModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0, 1, -30);
    /* ---------------------- Player ---------------------- */

    /* ---------------------- Floor ---------------------- */
    entity = scene->AddEntity("floor");
    mc = entity->AddComponent<component::ModelComponent>();
    tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(floorModel);
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(35, 1, 35);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
    /* ---------------------- Floor ---------------------- */

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