#include "GameNetwork.h"

GameNetwork::GameNetwork()
{
    EventBus::GetInstance().Subscribe(this, &GameNetwork::addNewPlayerEntity);
    EventBus::GetInstance().Subscribe(this, &GameNetwork::connectToServer);
}

void GameNetwork::SetScene(Scene* scene)
{
    m_pScene = scene;
}

void GameNetwork::SetNetwork(Network* network)
{
    m_pNetwork = network;
}

void GameNetwork::SetSceneManager(SceneManager* sceneManager)
{
    m_pSceneManager = sceneManager;
}

void GameNetwork::connectToServer(ConnectToServer* evnt)
{
    m_pNetwork->SetPlayerEntityPointer(m_pScene->GetEntity("player"), 0);
    m_pNetwork->ConnectToIP(evnt->ip, std::atoi(Option::GetInstance().GetVariable("i_port").c_str()));
}

void GameNetwork::addNewPlayerEntity(PlayerConnection* evnt)
{
    Log::Print("New player connected with ID " + std::to_string(evnt->playerId) + "\n");

    Entity* entity = m_pScene->AddEntity("player" + std::to_string(evnt->playerId));
    component::ModelComponent* mc = entity->AddComponent<component::ModelComponent>();
    component::TransformComponent* tc = entity->AddComponent<component::TransformComponent>();
    component::CubeCollisionComponent* bcc = entity->AddComponent<component::CubeCollisionComponent>(1.0f, 1.0f, 1.0f, 1.0f, 0.01f, 0.0f);

    mc = entity->GetComponent<component::ModelComponent>();
    mc->SetModel(AssetLoader::Get()->LoadModel(L"../Vendor/Resources/Models/Player/player.obj"));
    mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
    tc = entity->GetComponent<component::TransformComponent>();
    tc->GetTransform()->SetScale(1.0f);
    tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

    m_pNetwork->SetPlayerEntityPointer(entity, evnt->playerId);

    // TODO: Network setscene/addentity correct
    m_pSceneManager->AddEntity(entity, m_pSceneManager->GetActiveScenes()->at(0));
    m_pSceneManager->SetScenes(1, &m_pScene);
}
