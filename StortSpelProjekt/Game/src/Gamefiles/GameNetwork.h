#ifndef GAME_NETWORK_H
#define GAME_NETWORK_H

#include "Engine.h"

class GameNetwork {
public:
	GameNetwork();

	void Update(double dt);

	void SetEnemies(std::vector<Entity*>* enemyVector);
	void SetScenes(std::vector<Scene*>* activeScenes);
	void SetNetwork(Network* network);
	void SetSceneManager(SceneManager* sceneManager);

private:
	SceneManager* m_pSceneManager;
	std::vector<Scene*>* m_pActiveScenes;
	Network* m_pNetwork;
	std::vector<Entity*>* m_pEnemies;

	void disconnect(Disconnect* evnt);
	void connectToServer(ConnectToServer* evnt);
	void addNewPlayerEntity(PlayerConnection* evnt);

};

#endif // !GAME_NETWORK_H
