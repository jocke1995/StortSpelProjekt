#ifndef SCENE_H
#define SCENE_H

#include "Core.h"
#include "EngineMath.h"
class Entity;
class BaseCamera;
class NavMesh;
class SceneManager;

#include <map>
#include <vector>

class Scene
{
public:
	Scene(std::string sceneName);
	virtual ~Scene();

	Entity* AddEntityFromOther(Entity* other);
	Entity* AddEntity(std::string entityName);

	bool RemoveEntity(std::string entityName);

	NavMesh* CreateNavMesh(const std::string& type);

	void SetPrimaryCamera(BaseCamera* primaryCamera);

	Entity* GetEntity(std::string entityName);
	const std::map<std::string, Entity*>* GetEntities() const;
	bool EntityExists(std::string entityName) const;

	unsigned int GetNrOfEntites() const;
	BaseCamera* GetMainCamera() const;
	std::string GetName() const;

	NavMesh* GetNavMesh();

	void SetUpdateScene(void(*UpdateScene)(SceneManager*, double dt));

	void Update(SceneManager* sm, double dt);
	void RenderUpdate(SceneManager* sm, double dt);

	void SetCollisionEntities(const std::vector<Entity*>* collisionEntities);
	const std::vector<Entity*>* GetCollisionEntities() const;

	// Init function to be called after all components have been initialized.
	void OnInit();
	void SetOnInit(void (*OnInit)(Scene*));
private:
	friend class SceneManager;

	std::string m_SceneName;

	std::map<std::string, Entity*> m_EntitiesToKeep;

	std::vector<Entity*> m_CollisionEntities;
	unsigned int m_NrOfEntities = 0;

	BaseCamera* m_pPrimaryCamera = nullptr;

	NavMesh* m_pNavMesh;


	// Every scene has its own functionpointer to a updateSceneFunction in main
	void(*m_pUpdateScene)(SceneManager*, double dt);
	float3 m_OriginalPosition = {};
	// Every scene has its own functionpointer to a inifunction. By default it does nothing.
	void (*m_pOnInit)(Scene*);
};

#endif
