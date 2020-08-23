#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

#include "../src/Engine/BaseCamera.h"
class Scene
{
public:
	Scene(std::string sceneName);
	~Scene();

	Entity* AddEntityFromOther(Entity* other);
	Entity* AddEntity(std::string entityName);
	bool RemoveEntity(std::string entityName);

	void SetPrimaryCamera(BaseCamera* primaryCamera);

	Entity* GetEntity(std::string entityName);
	const std::map<std::string, Entity*> *GetEntities() const;

	unsigned int GetNrOfEntites() const;
	BaseCamera* GetMainCamera() const;
	std::string GetName() const;

	void UpdateScene(double dt);
	
private:
	std::string sceneName;
	std::map<std::string, Entity*> entities;
	unsigned int nrOfEntities = 0;

	BaseCamera* primaryCamera = nullptr;

	bool EntityExists(std::string entityName) const;
};

#endif
