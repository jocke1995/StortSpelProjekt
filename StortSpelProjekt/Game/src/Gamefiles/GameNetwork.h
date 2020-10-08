#ifndef GAME_NETWORK_H
#define GAME_NETWORK_H

#include "Engine.h"

class GameNetwork {
public:
	GameNetwork();

	void SetScenes(std::vector<Scene*>* activeScenes);
	void SetNetwork(Network* network);
	void SetSceneManager(SceneManager* sceneManager);

private:
	SceneManager* m_pSceneManager;
	std::vector<Scene*>* m_pActiveScenes;
	Network* m_pNetwork;

	void connectToServer(ConnectToServer* evnt);
	void addNewPlayerEntity(PlayerConnection* evnt);

};

#endif // !GAME_NETWORK_H
