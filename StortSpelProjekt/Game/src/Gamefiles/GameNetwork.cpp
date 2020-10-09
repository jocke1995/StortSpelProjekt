#include "GameNetwork.h"

GameNetwork::GameNetwork()
{
    EventBus::GetInstance().Subscribe(this, &GameNetwork::addNewPlayerEntity);
    EventBus::GetInstance().Subscribe(this, &GameNetwork::connectToServer);
    EventBus::GetInstance().Subscribe(this, &GameNetwork::disconnect);
}

void GameNetwork::Update()
{
    m_pNetwork->SendPositionPacket();
    std::vector<float3> positionList;
    std::vector<float3> movementList;
    for (int i = 0; i < m_pScene->GetEntity("player")->GetComponent<component::RangeComponent>()->GetProjectileList().size(); i++)
    {
        Transform* temp = m_pScene->GetEntity("player")->GetComponent<component::RangeComponent>()->GetProjectileList().at(i)->GetComponent<component::TransformComponent>()->GetTransform();
        float3 position = temp->GetPositionFloat3();
        float3 movement = 
        {
           temp->GetMovement().x,
           temp->GetMovement().y,
           temp->GetMovement().z,
        };
        positionList.push_back(position);
        movementList.push_back(movement);
        
    }
    m_pNetwork->SendRangedAttackPacket(positionList, movementList);
    m_pScene->GetEntity("player")->GetComponent<component::RangeComponent>()->ClearProjectileList();
    while (m_pNetwork->ListenPacket());
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

void GameNetwork::disconnect(Disconnect* evnt)
{
    m_pNetwork->Disconnect();
}

void GameNetwork::connectToServer(ConnectToServer* evnt)
{
    m_pNetwork->SetPlayerEntityPointer(m_pScene->GetEntity("player"), 0);
    m_pNetwork->ConnectToIP(evnt->ip, std::atoi(Option::GetInstance().GetVariable("i_port").c_str()));
}

void GameNetwork::addNewPlayerEntity(PlayerConnection* evnt)
{
    Log::Print("New player connected with ID " + std::to_string(evnt->playerId) + "\n");
    
    Entity* entity = nullptr;
    if (!m_pScene->EntityExists("player" + std::to_string(evnt->playerId)))
    {
        entity = m_pScene->AddEntity("player" + std::to_string(evnt->playerId));
        component::ModelComponent* mc = entity->AddComponent<component::ModelComponent>();
        component::TransformComponent* tc = entity->AddComponent<component::TransformComponent>();
        component::CubeCollisionComponent* bcc = entity->AddComponent<component::CubeCollisionComponent>(1.0f, 1.0f, 1.0f, 1.0f, 0.01f, 0.0f);

        mc = entity->GetComponent<component::ModelComponent>();
        mc->SetModel(AssetLoader::Get()->LoadModel(L"../Vendor/Resources/Models/Player/player.obj"));
        mc->SetDrawFlag(FLAG_DRAW::DRAW_OPAQUE | FLAG_DRAW::GIVE_SHADOW);
        tc = entity->GetComponent<component::TransformComponent>();
        tc->GetTransform()->SetScale(1.0f);
        tc->GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

        m_pSceneManager->AddEntity(entity);
    }
    else
    {
        entity = m_pScene->GetEntity("player" + std::to_string(evnt->playerId));
    }

    m_pNetwork->SetPlayerEntityPointer(entity, evnt->playerId);
}
