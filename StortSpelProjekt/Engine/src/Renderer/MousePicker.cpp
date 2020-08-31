#include "stdafx.h"
#include "MousePicker.h"

MousePicker::MousePicker()
{

}

MousePicker::~MousePicker()
{

}

void MousePicker::SetPrimaryCamera(BaseCamera* primaryCamera)
{
	this->primaryCamera = primaryCamera;
}

void MousePicker::UpdateRay()
{
	// Pick from middle of the screen
	this->rayInWorldSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	this->rayInWorldSpaceDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Transform ray to worldSpace by taking inverse of the view matrix
	this->rayInWorldSpacePos = XMVector3TransformCoord(this->rayInWorldSpacePos, *this->primaryCamera->GetViewMatrixInverse());
	this->rayInWorldSpaceDir = XMVector3TransformNormal(this->rayInWorldSpaceDir, *this->primaryCamera->GetViewMatrixInverse());
}

bool MousePicker::Pick(component::BoundingBoxComponent* bbc, float& distance)
{
	const BoundingBoxData* bbd = bbc->GetBoundingBoxData();
	distance = -1;
	std::vector<Vertex> vertices = bbd->boundingBoxVertices;
	std::vector<unsigned int> indices = bbd->boundingBoxIndices;
	XMMATRIX worldMatrix = *bbc->GetTransform()->GetWorldMatrix();

	// For each triangle
	for (unsigned int i = 0; i < indices.size() / 3; i++)
	{
		// Triangle's vertices V1, V2, V3
		XMVECTOR tri1V1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V3 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		// For each vertex in the triangle
		XMFLOAT4 v1, v2, v3;

		// Get triangle 
		v1 = vertices[indices[(i * 3) + 0]].pos;
		v2 = vertices[indices[(i * 3) + 1]].pos;
		v3 = vertices[indices[(i * 3) + 2]].pos;

		tri1V1 = XMVectorSet(v1.x, v1.y, v1.z, 0.0f);
		tri1V2 = XMVectorSet(v2.x, v2.y, v2.z, 0.0f);
		tri1V3 = XMVectorSet(v3.x, v3.y, v3.z, 0.0f);

		// Transform the vertices to world space
		tri1V1 = XMVector3TransformCoord(tri1V1, worldMatrix);
		tri1V2 = XMVector3TransformCoord(tri1V2, worldMatrix);
		tri1V3 = XMVector3TransformCoord(tri1V3, worldMatrix);

		// Find the normal using U, V coordinates (two edges)
		XMVECTOR U = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR V = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR faceNormal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = tri1V2 - tri1V1;
		V = tri1V3 - tri1V1;

		// Compute face normal by crossing U, V
		faceNormal = XMVector3Cross(U, V);

		faceNormal = XMVector3Normalize(faceNormal);

		// Calculate a point on the triangle for the plane equation
		XMVECTOR triPoint = tri1V1;

		// Get plane equation ("Ax + By + Cz + D = 0") Variables
		// (" D = -Ax -By -Cz")
		float tri1A = XMVectorGetX(faceNormal);
		float tri1B = XMVectorGetY(faceNormal);
		float tri1C = XMVectorGetZ(faceNormal);
		float tri1D = (-tri1A * XMVectorGetX(triPoint) - tri1B * XMVectorGetY(triPoint) - tri1C * XMVectorGetZ(triPoint));

		// Now we find where (on the ray) the ray intersects with the triangles plane
		float ep1, ep2, t = 0.0f;
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
		XMVECTOR pointInPlane = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		ep1 = (tri1A * XMVectorGetX(this->rayInWorldSpacePos)) + (tri1B * XMVectorGetY(this->rayInWorldSpacePos)) + (tri1C * XMVectorGetZ(this->rayInWorldSpacePos));
		ep2 = (tri1A * XMVectorGetX(this->rayInWorldSpaceDir)) + (tri1B * XMVectorGetY(this->rayInWorldSpaceDir)) + (tri1C * XMVectorGetZ(this->rayInWorldSpaceDir));

		// Make sure there are no divide-by-zeros
		if (ep2 != 0.0f)
			t = -(ep1 + tri1D) / (ep2);

		// Make sure you not to pick objects behind the camera
		if (t > 0.0f)    
		{
			// Get the point on the plane
			planeIntersectX = XMVectorGetX(this->rayInWorldSpacePos) + XMVectorGetX(this->rayInWorldSpaceDir) * t;
			planeIntersectY = XMVectorGetY(this->rayInWorldSpacePos) + XMVectorGetY(this->rayInWorldSpaceDir) * t;
			planeIntersectZ = XMVectorGetZ(this->rayInWorldSpacePos) + XMVectorGetZ(this->rayInWorldSpaceDir) * t;

			pointInPlane = XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			// Call function to check if point is in the triangle
			if (this->IsPointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
			{
				// Return the distance to the hit, so you can check all the other pickable objects in your scene
				// and choose whichever object is closest to the camera
				distance = t / 2.0f;
				return true;
			}
		}
	}
	return false;
}

bool MousePicker::IsPointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point)
{
	// To find out if the point is inside the triangle, we will check to see if the point
	// is on the correct side of each of the triangles edges.

	XMVECTOR cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
	XMVECTOR cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
	if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
		cp2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
		if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = XMVector3Cross((triV2 - triV1), (point - triV1));
			cp2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
			if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
			{
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	return false;
}
