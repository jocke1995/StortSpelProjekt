#include "stdafx.h"
#include "Engine.h"
#include "Misc/Thread.h"
Engine::Engine()
{
	
}

Engine::~Engine()
{
	delete m_pWindow;
	delete m_pTimer;

	m_pThreadPool->WaitForThreads(FLAG_THREAD::ALL);
	m_pThreadPool->ExitThreads();
	delete m_pThreadPool;

	delete m_pSceneManager;
	delete m_pRenderer;
	delete m_pAudioEngine;
}

void Engine::Init(HINSTANCE hInstance, int nCmdShow)
{
	// Misc
	m_pWindow = new Window(hInstance, nCmdShow, false);
	m_pTimer = new Timer(m_pWindow);

	// ThreadPool
	int numCores = std::thread::hardware_concurrency();
	if (numCores == 0) numCores = 1; // function not supported
	m_pThreadPool = new ThreadPool(numCores); // Set num m_Threads to number of cores of the cpu

	// Sub-engines
	m_pRenderer = new Renderer();
	m_pRenderer->InitD3D12(m_pWindow, hInstance, m_pThreadPool);

	m_pAudioEngine = new AudioEngine();

	// ECS
	m_pSceneManager = new SceneManager(m_pRenderer);

	// Physics
	m_pPhysics = &Physics::GetInstance();

	Input::GetInstance().RegisterDevices(m_pWindow->GetHwnd());
}

Window* const Engine::GetWindow() const
{
	return m_pWindow;
}

Timer* const Engine::GetTimer() const
{
	return m_pTimer;
}

ThreadPool* const Engine::GetThreadPool() const
{
	return m_pThreadPool;
}

SceneManager* const Engine::GetSceneHandler() const
{
	return m_pSceneManager;
}

Physics* const Engine::GetPhysics() const
{
	return m_pPhysics;
}

AudioEngine* const Engine::GetAudioEngine() const
{
	return m_pAudioEngine;
}

Renderer* const Engine::GetRenderer() const
{
	return m_pRenderer;
}
