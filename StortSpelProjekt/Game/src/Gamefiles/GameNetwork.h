#ifndef GAME_NETWORK_H
#define GAME_NETWORK_H

#include "Engine.h"

class GameNetwork {
public:
	GameNetwork();

	void SetScene(Scene* scene);
	void SetNetwork(Network* network);
	void SetSceneManager(SceneManager* sceneManager);

private:
	SceneManager* m_pSceneManager;
	Scene* m_pScene;
	Network* m_pNetwork;

	void addNewPlayerEntity(PlayerConnection* evnt);

};

#endif // !GAME_NETWORK_H
