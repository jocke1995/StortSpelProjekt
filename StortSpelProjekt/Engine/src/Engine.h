#ifndef ENGINE_H
#define ENGINE_H

// Miscellaneous
#include "Misc/Window.h"
#include "Misc/Timer.h"
#include "Misc/ThreadPool.h"

// Entity Component System
#include "ECS/SceneManager.h"
#include "ECS/Entity.h"
#include "ECS/Components/MeshComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/Lights/DirectionalLightComponent.h"
#include "ECS/Components/Lights/PointLightComponent.h"
#include "ECS/Components/Lights/SpotLightComponent.h"
#include "ECS/Components/CameraComponent.h"

// Sub-engines
#include "Renderer/Renderer.h"
#include "Renderer/Transform.h"
#include "Renderer/Mesh.h"
#include "Renderer/BaseCamera.h"
// #include "physics"
// #include "audio"

#include "Misc/AssetLoader.h"

class Engine
{
public:
	Engine();
	~Engine();

	void Init(HINSTANCE hInstance, int nCmdShow);

	Window* const GetWindow() const;
	Timer* const GetTimer() const;
	ThreadPool* const GetThreadPool() const;

	Renderer* const GetRenderer() const;
	SceneManager* const GetSceneHandler() const;

private:
	Window* m_Window = nullptr;
	Timer* m_Timer = nullptr;
	ThreadPool* m_ThreadPool = nullptr;

	Renderer* m_Renderer = nullptr;
	SceneManager* m_SceneManager = nullptr;
};

#endif
