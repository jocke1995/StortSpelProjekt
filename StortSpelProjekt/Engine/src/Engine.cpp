#include "stdafx.h"
#include "Engine.h"
#include "Misc/MultiThreading/Thread.h"

Engine::Engine()
{
	
}

Engine::~Engine()
{
	// Gpu will crash if we delete stuff while commandQueues are running
	m_pRenderer->waitForGPU();

	delete m_pWindow;
	delete m_pTimer;

	Physics::GetInstance().DestroyPhysics();
	m_pSceneManager->EraseSceneManager();
	m_pRenderer->DeleteDxResources();
}

void Engine::Init(HINSTANCE hInstance, int nCmdShow)
{
	// Window values
	bool windowedFullscreen = false;
	if (std::atoi(Option::GetInstance().GetVariable("i_windowMode").c_str()) == static_cast<int>(WINDOW_MODE::WINDOWED_FULLSCREEN))
	{
		windowedFullscreen = true;
	}
	int windowWidth = std::atoi(Option::GetInstance().GetVariable("i_windowWidth").c_str());
	int windowHeight = std::atoi(Option::GetInstance().GetVariable("i_windowHeight").c_str());

	// Misc
	m_pWindow = new Window(hInstance, nCmdShow, windowedFullscreen, windowWidth, windowHeight);
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
	m_pThreadPool = &ThreadPool::GetInstance(numThreads);

	// Sub-engines
	m_pRenderer = &Renderer::GetInstance();
	m_pRenderer->InitD3D12(m_pWindow, hInstance, m_pThreadPool);

	// Audio engine
	m_pAudioEngine = &AudioEngine::GetInstance();

	// ECS
	m_pSceneManager = &SceneManager::GetInstance();

	// Physics
	m_pPhysics = &Physics::GetInstance();

	m_pParticleSystem = &ParticleSystem::GetInstance();

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
