#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include "Engine.h"
#include "PlayerInputComponent.h"

// Temporary
#include "SandboxComponent.h"

class GameEntity : public Entity
{
public:
	GameEntity(std::string entityName);
	virtual ~GameEntity();
};

#endif
