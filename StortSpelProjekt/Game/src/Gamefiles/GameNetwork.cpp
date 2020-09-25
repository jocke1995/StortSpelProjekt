#include "GameNetwork.h"

GameNetwork::GameNetwork()
{
    EventBus::GetInstance().Subscribe(this, &GameNetwork::addNewPlayerEntity);
}

void GameNetwork::SetScene(Scene* scene)
{
    m_Scene = scene;
}

void GameNetwork::SetNetwork(Network* network)
{
    m_Network = network;
}

void GameNetwork::SetSceneManager(SceneManager* sceneManager)
{
    m_SceneManager = sceneManager;
}

void GameNetwork::addNewPlayerEntity(PlayerConnection* evnt)
{
    Log::Print("New player connected with ID " + std::to_string(evnt->playerId) + "\n");

    Entity* entity = m_Scene->AddEntity("player" + std::to_string(evnt->playerId));
    component::ModelComponent* mc = entity->AddComponent<component::ModelComponent>();
    component::TransformComponent* tc = entity->AddComponent<component::TransformComponent>();

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(AssetLoader::Get()->LoadModel(L"../Vendor/Resources/Models/Player/player.obj"));
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

    m_Network->SetPlayerEntityPointer(entity, evnt->playerId);
    m_SceneManager->SetSceneToDraw(m_Scene);
}
