#ifndef ENGINE_H
#define ENGINE_H

// Miscellaneous
#include "Misc/Window.h"
#include "Misc/Timer.h"
#include "Misc/ThreadPool.h"

// Entity Component System
#include "ECS/SceneHandler.h"

// Sub-engines
#include "Renderer/Renderer.h"
// #include "physics"
// #include "audio"

class Engine
{
public:
	Engine();
	~Engine();

	void Init(HINSTANCE hInstance, int nCmdShow);

	Window* const GetWindow() const;
	Timer* const GetTimer() const;
	ThreadPool* const GetThreadPool() const;

	SceneHandler* const GetSceneHandler() const;
	Renderer* const GetRenderer() const;

private:
	Window* window = nullptr;
	Timer* timer = nullptr;
	ThreadPool* threadPool = nullptr;

	SceneHandler* sceneHandler = nullptr;
	Renderer* renderer = nullptr;
};

#endif
