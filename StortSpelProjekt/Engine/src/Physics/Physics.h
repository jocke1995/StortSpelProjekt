#ifndef PHYSICS_H
#define PHYSICS_H

#include<DirectXCollision.h>

class Physics
{
public:
	Physics();
	~Physics();

	// returns true if there is an intersection between the OBBs
	bool checkOBBCollision(DirectX::BoundingOrientedBox oBB1, DirectX::BoundingOrientedBox oBB2);

private:

};

#endif
