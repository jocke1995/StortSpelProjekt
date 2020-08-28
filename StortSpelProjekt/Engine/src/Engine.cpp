#include "Engine.h"

Engine::Engine()
{
	
}

Engine::~Engine()
{
	delete this->window;
	delete this->timer;

	this->threadPool->WaitForThreads(FLAG_THREAD::ALL);
	this->threadPool->ExitThreads();
	delete this->threadPool;

	delete this->sceneHandler;
	delete this->renderer;
}

void Engine::Init(HINSTANCE hInstance, int nCmdShow)
{
	// Misc
	this->window = new Window(hInstance, nCmdShow, false);
	this->timer = new Timer(this->window);

	// ThreadPool
	int numCores = std::thread::hardware_concurrency();
	if (numCores == 0) numCores = 1; // function not supported
	this->threadPool = new ThreadPool(numCores); // Set num threads to number of cores of the cpu

	// Sub-engines
	this->renderer = new Renderer();
	this->renderer->InitD3D12(this->window->GetHwnd(), hInstance, this->threadPool);

	// ECS
	this->sceneHandler = new SceneManager(this->renderer);
}

Window* const Engine::GetWindow() const
{
	return this->window;
}

Timer* const Engine::GetTimer() const
{
	return this->timer;
}

ThreadPool* const Engine::GetThreadPool() const
{
	return this->threadPool;
}

SceneManager* const Engine::GetSceneHandler() const
{
	return this->sceneHandler;
}

Renderer* const Engine::GetRenderer() const
{
	return this->renderer;
}
