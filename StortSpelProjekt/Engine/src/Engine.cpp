#include "stdafx.h"
#include "Engine.h"

Engine::Engine()
{
	
}

Engine::~Engine()
{
	delete this->m_Window;
	delete this->m_Timer;

	this->m_ThreadPool->WaitForThreads(FLAG_THREAD::ALL);
	this->m_ThreadPool->ExitThreads();
	delete this->m_ThreadPool;

	delete this->m_SceneManager;
	delete this->m_Renderer;
}

void Engine::Init(HINSTANCE hInstance, int nCmdShow)
{
	// Misc
	this->m_Window = new Window(hInstance, nCmdShow, false);
	this->m_Timer = new Timer(this->m_Window);

	// ThreadPool
	int numCores = std::thread::hardware_concurrency();
	if (numCores == 0) numCores = 1; // function not supported
	this->m_ThreadPool = new ThreadPool(numCores); // Set num threads to number of cores of the cpu

	// Sub-engines
	this->m_Renderer = new Renderer();
	this->m_Renderer->InitD3D12(this->m_Window->GetHwnd(), hInstance, this->m_ThreadPool);

	// ECS
	this->m_SceneManager = new SceneManager(this->m_Renderer);
}

Window* const Engine::GetWindow() const
{
	return this->m_Window;
}

Timer* const Engine::GetTimer() const
{
	return this->m_Timer;
}

ThreadPool* const Engine::GetThreadPool() const
{
	return this->m_ThreadPool;
}

SceneManager* const Engine::GetSceneHandler() const
{
	return this->m_SceneManager;
}

Renderer* const Engine::GetRenderer() const
{
	return this->m_Renderer;
}
