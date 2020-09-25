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
}

void Engine::Init(HINSTANCE hInstance, int nCmdShow)
{
	// Window values
	bool fullscreen = Option::GetInstance().GetVariable("fullscreen");
	int windowWidth = Option::GetInstance().GetVariable("windowWidth");
	int windowHeight = Option::GetInstance().GetVariable("windowHeight");

	// Misc
	m_pWindow = new Window(hInstance, nCmdShow, fullscreen, windowWidth, windowHeight);
	m_pTimer = new Timer(m_pWindow);

	// ThreadPool
	int numThreads = std::thread::hardware_concurrency();
	if (numThreads == 0) // function not supported
	{
		numThreads = 1;
	}
	else if (numThreads > m_ThreadLimit) // Limiting the number of threads to the threadLimit
	{
		numThreads = m_ThreadLimit;
	}
	m_pThreadPool = new ThreadPool(numThreads); // Set num m_Threads to number of cores of the cpu

	// Sub-engines
	m_pRenderer = new Renderer();
	m_pRenderer->InitD3D12(m_pWindow, hInstance, m_pThreadPool);

	// Audio engine
	m_pAudioEngine = &AudioEngine::GetInstance();

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
