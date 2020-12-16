#include "CreditsSceneHandler.h"
#include "ECS/SceneManager.h"
#include "ECS/Entity.h"
#include "Misc/AssetLoader.h"
#include "Events/EventBus.h"
#include "Events/Events.h"
#include "Renderer/Texture/Texture2D.h"
#include "Misc/GUI2DElements/Font.h"

void onUpScroller();
void onDownScroller();

CreditsSceneHandler::CreditsSceneHandler()
{

}

CreditsSceneHandler& CreditsSceneHandler::GetInstance()
{
    static CreditsSceneHandler instance;
    return instance;
}

CreditsSceneHandler::~CreditsSceneHandler()
{
}

Scene* CreditsSceneHandler::CreateScene(SceneManager* sm)
{
    m_pScene = sm->CreateScene("creditsScene");

    component::GUI2DComponent* guic = nullptr;
    component::Audio2DVoiceComponent* vc = nullptr;

    // Player, since we always need one...
    Entity* entity = m_pScene->AddEntity("player");
    entity->AddComponent<component::CameraComponent>(CAMERA_TYPE::PERSPECTIVE, true);
    // Add a voice to the player to play some music.
    vc = entity->AddComponent<component::Audio2DVoiceComponent>();
    vc->AddVoice(L"MenuMusic");

    // Up scroller

    // Down scroller

    // Text, all of it!
    return nullptr;
}

Scene* CreditsSceneHandler::GetScene()
{
    return nullptr;
}
