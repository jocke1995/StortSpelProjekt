#ifndef SCENE_H
#define SCENE_H

#include <map>
#include "Core.h"
class Entity;
class BaseCamera;
class NavMesh;

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
	const std::map<std::string, Entity*> *GetEntities() const;
	bool EntityExists(std::string entityName) const;

	unsigned int GetNrOfEntites() const;
	BaseCamera* GetMainCamera() const;
	std::string GetName() const;

	NavMesh* GetNavMesh();

	void Update(double dt);
	void RenderUpdate(double dt);
	
private:
	friend class SceneManager;

	std::string m_SceneName;

	std::map<std::string, Entity*> m_Entities;
	unsigned int m_NrOfEntities = 0;

	BaseCamera* m_pPrimaryCamera = nullptr;

	NavMesh* m_pNavMesh;
};

#endif
