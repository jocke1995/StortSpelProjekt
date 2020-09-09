#include "stdafx.h"
#include "Engine.h"
#include "Misc/Thread.h"
Engine::Engine()
{
	
}

Engine::~Engine()
{
	delete m_Window;
	delete m_Timer;

	m_ThreadPool->WaitForThreads(FLAG_THREAD::ALL);
	m_ThreadPool->ExitThreads();
	delete m_ThreadPool;

	delete m_SceneManager;
	delete m_Renderer;
	delete m_pAudioEngine;
}

void Engine::Init(HINSTANCE hInstance, int nCmdShow)
{
	// Misc
	m_Window = new Window(hInstance, nCmdShow, false);
	m_Timer = new Timer(m_Window);

	// ThreadPool
	int numCores = std::thread::hardware_concurrency();
	if (numCores == 0) numCores = 1; // function not supported
	m_ThreadPool = new ThreadPool(numCores); // Set num m_Threads to number of cores of the cpu

	// Sub-engines
	m_Renderer = new Renderer();
	m_Renderer->InitD3D12(m_Window->GetHwnd(), hInstance, m_ThreadPool);

	m_pAudioEngine = new AudioEngine();

	// ECS
	m_SceneManager = new SceneManager(m_Renderer);
}

Window* const Engine::GetWindow() const
{
	return m_Window;
}

Timer* const Engine::GetTimer() const
{
	return m_Timer;
}

ThreadPool* const Engine::GetThreadPool() const
{
	return m_ThreadPool;
}

SceneManager* const Engine::GetSceneHandler() const
{
	return m_SceneManager;
}

IXAudio2* const Engine::GetAudioEngine() const
{
	return m_pAudioEngine->GetAudioEngine();
}

Renderer* const Engine::GetRenderer() const
{
	return m_Renderer;
}
