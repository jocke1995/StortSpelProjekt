#include "GameNetwork.h"

#include "ECS/SceneManager.h"

GameNetwork::GameNetwork()
{
    EventBus::GetInstance().Subscribe(this, &GameNetwork::addNewPlayerEntity);
    EventBus::GetInstance().Subscribe(this, &GameNetwork::connectToServer);
    EventBus::GetInstance().Subscribe(this, &GameNetwork::disconnect);
}

void GameNetwork::Update(double dt)
{
    m_pNetwork->SendPositionPacket();
    m_pNetwork->SendEnemiesPacket(m_pEnemies);
    while (m_pNetwork->ListenPacket());
}

void GameNetwork::SetScenes(std::vector<Scene*>* activeScenes)
{
    m_pActiveScenes = activeScenes;
}

void GameNetwork::SetEnemies(std::vector<Entity*>* enemyVector)
{
    m_pEnemies = enemyVector;
}

void GameNetwork::SetNetwork(Network* network)
{
    m_pNetwork = network;
}

void GameNetwork::SetSceneManager(SceneManager* sceneManager)
{
    m_pSceneManager = sceneManager;
}

void GameNetwork::disconnect(Disconnect* evnt)
{
    m_pNetwork->Disconnect();
}

void GameNetwork::connectToServer(ConnectToServer* evnt)
{
    m_pNetwork->SetPlayerEntityPointer((*m_pActiveScenes).at(0)->GetEntity("player"), 0);
    m_pNetwork->ConnectToIP(evnt->ip, std::atoi(Option::GetInstance().GetVariable("i_port").c_str()));
}

void GameNetwork::addNewPlayerEntity(PlayerConnection* evnt)
{
    Log::Print("New player connected with ID " + std::to_string(evnt->playerId) + "\n");
    
    std::vector<Scene*>* activeScenes = m_pSceneManager->GetActiveScenes();


    Scene* scene0 = (*activeScenes).at(0);
    Entity* entity = nullptr;
    if (!scene0->EntityExists("player" + std::to_string(evnt->playerId)))
    {
        entity = new Entity("player" + std::to_string(evnt->playerId));
        component::ModelComponent* mc = entity->AddComponent<component::ModelComponent>();
        component::TransformComponent* tc = entity->AddComponent<component::TransformComponent>();
        component::CubeCollisionComponent* bcc = entity->AddComponent<component::CubeCollisionComponent>(1.0f, 1.0f, 1.0f, 1.0f, 0.01f, 0.0f);

        mc = entity->GetComponent<component::ModelComponent>();
        mc->SetModel(AssetLoader::Get()->LoadModel(L"../Vendor/Resources/Models/Player/player.obj"));
        mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
        tc = entity->GetComponent<component::TransformComponent>();
        tc->GetTransform()->SetScale(1.0f);
        tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

        m_pSceneManager->AddEntity(entity, scene0);
    }
    else
    {
        entity = scene0->GetEntity("player" + std::to_string(evnt->playerId));
    }

    m_pNetwork->SetPlayerEntityPointer(entity, evnt->playerId);

    m_pSceneManager->SetScenes(m_pSceneManager->GetActiveScenes()->size(), &activeScenes->at(0));
}
