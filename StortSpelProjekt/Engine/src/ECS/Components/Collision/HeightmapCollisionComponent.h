#ifndef HEIGHTMAPCOLLISIONCOMPONENT_H
#define HEIGHTMAPCOLLISIONCOMPONENT_H

#include "CollisionComponent.h"

struct HeightMapInfo
{
	const void* data;
	int width;
	int length;
	double maxHeight;
	double minHeight;
};

namespace component
{
	class HeightmapCollisionComponent : public CollisionComponent
	{
	public:
		HeightmapCollisionComponent(Entity* parent, HeightMapInfo info, double mass = 0.0, double friction = 1.0, double restitution = 0.5);
		~HeightmapCollisionComponent();
		double GetDistanceToBottom() const;
	private:
	};
}

#endif