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
    m_Network.SendPositionPacket();
    if (m_Network.IsHost())
    {
        m_Network.SendEnemiesPacket(m_pEnemies);
    }
    while (m_Network.ListenPacket());
}

void GameNetwork::SetScene(Scene* activeScene)
{
    m_pActiveScene = activeScene;
}

void GameNetwork::SetEnemies(std::vector<Entity*>* enemyVector)
{
    m_pEnemies = enemyVector;
    m_Network.SetEnemiesEntityPointers(enemyVector);
}

void GameNetwork::SetSceneManager(SceneManager* sceneManager)
{
    m_pSceneManager = sceneManager;
}

bool GameNetwork::IsConnected()
{
    return m_Network.IsConnected();
}

void GameNetwork::disconnect(Disconnect* evnt)
{
    m_Network.Disconnect();
}

void GameNetwork::connectToServer(ConnectToServer* evnt)
{
    m_Network.SetPlayerEntityPointer(m_pActiveScene->GetEntity("player"), 0);
    m_Network.ConnectToIP(evnt->ip, std::atoi(Option::GetInstance().GetVariable("i_port").c_str()));
}

void GameNetwork::addNewPlayerEntity(PlayerConnection* evnt)
{
    Log::Print("New player connected with ID " + std::to_string(evnt->playerId) + "\n");
    
    Scene* activeScene = m_pSceneManager->GetActiveScene();

    Entity* entity = nullptr;
    if (!activeScene->EntityExists("player" + std::to_string(evnt->playerId)))
    {
        entity = new Entity("player" + std::to_string(evnt->playerId));
        component::ModelComponent* mc = entity->AddComponent<component::ModelComponent>();
        component::TransformComponent* tc = entity->AddComponent<component::TransformComponent>();
        component::CubeCollisionComponent* bcc = entity->AddComponent<component::CubeCollisionComponent>(1.0f, 1.0f, 1.0f, 1.0f, 0.01f, 0.0f);

        mc = entity->GetComponent<component::ModelComponent>();
        mc->SetModel(AssetLoader::Get()->LoadModel(L"../Vendor/Resources/Models/Man/man.obj"));
        mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
        tc = entity->GetComponent<component::TransformComponent>();
        tc->GetTransform()->SetScale(1.0f);
        tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

        activeScene->InitDynamicEntity(entity);
    }
    else
    {
        entity = activeScene->GetEntity("player" + std::to_string(evnt->playerId));
    }

    m_Network.SetPlayerEntityPointer(entity, evnt->playerId);
    for (int i = 0; i < m_pEnemies->size(); i++)
    {
        m_pEnemies->at(i)->GetComponent<component::AiComponent>()->AddTarget(entity);
    }

    m_pSceneManager->SetScene(activeScene);
}
