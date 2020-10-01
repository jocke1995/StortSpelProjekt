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

	delete m_pThreadPool;
	Physics::GetInstance().DestroyPhysics();
	delete m_pSceneManager;
	m_pRenderer->DeleteDxResources();
}

void Engine::Init(HINSTANCE hInstance, int nCmdShow)
{
	// Window values
	bool fullscreen = std::atoi(Option::GetInstance().GetVariable("i_fullscreen").c_str());
	int windowWidth = std::atoi(Option::GetInstance().GetVariable("i_windowWidth").c_str());
	int windowHeight = std::atoi(Option::GetInstance().GetVariable("i_windowHeight").c_str());

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
	m_pThreadPool = new ThreadPool(numThreads);

	// Sub-engines
	m_pRenderer = &Renderer::GetInstance();
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
