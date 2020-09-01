#include "Engine.h"

// Helps intellisense to understand that stdafx is included
//#include "Headers/stdafx.h"

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

    if (renderer->GetActiveScene())
    {
        while (!window->ExitWindow())
        {
            // Currently no scene set, hence the renderer should not be working.

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
