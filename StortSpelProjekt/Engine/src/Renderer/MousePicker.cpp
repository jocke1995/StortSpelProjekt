#include "stdafx.h"
#include "MousePicker.h"

#include "EngineMath.h"
#include "BaseCamera.h"
#include "../Renderer/Transform.h"
#include "../Renderer/Mesh.h"
#include "../Renderer/BoundingBoxPool.h"

MousePicker::MousePicker()
{

}

MousePicker::~MousePicker()
{

}

void MousePicker::SetPrimaryCamera(BaseCamera* primaryCamera)
{
	m_pPrimaryCamera = primaryCamera;
}

void MousePicker::UpdateRay()
{
	// Pick from middle of the screen
	m_RayInWorldSpacePos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_RayInWorldSpaceDir = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Transform ray to worldSpace by taking inverse of the view matrix
	m_RayInWorldSpacePos = DirectX::XMVector3TransformCoord(m_RayInWorldSpacePos, *m_pPrimaryCamera->GetViewMatrixInverse());
	m_RayInWorldSpaceDir = DirectX::XMVector3TransformNormal(m_RayInWorldSpaceDir, *m_pPrimaryCamera->GetViewMatrixInverse());
}

bool MousePicker::Pick(component::BoundingBoxComponent* bbc, float& distance)
{
	const BoundingBoxData* bbd = bbc->GetBoundingBoxData();
	distance = -1;
	std::vector<Vertex> vertices = bbd->boundingBoxVertices;
	std::vector<unsigned int> indices = bbd->boundingBoxIndices;
	DirectX::XMMATRIX worldMatrix = *bbc->GetTransform()->GetWorldMatrix();

	// For each triangle
	for (unsigned int i = 0; i < indices.size() / 3; i++)
	{
		// Triangle's vertices V1, V2, V3
		DirectX::XMVECTOR tri1V1 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR tri1V2 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR tri1V3 = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		// For each vertex in the triangle
		DirectX::XMFLOAT3 v1, v2, v3;

		// Get triangle 
		v1 = vertices[indices[(i * 3) + 0]].pos;
		v2 = vertices[indices[(i * 3) + 1]].pos;
		v3 = vertices[indices[(i * 3) + 2]].pos;

		tri1V1 = DirectX::XMVectorSet(v1.x, v1.y, v1.z, 0.0f);
		tri1V2 = DirectX::XMVectorSet(v2.x, v2.y, v2.z, 0.0f);
		tri1V3 = DirectX::XMVectorSet(v3.x, v3.y, v3.z, 0.0f);

		// Transform the vertices to world space
		tri1V1 = XMVector3TransformCoord(tri1V1, worldMatrix);
		tri1V2 = XMVector3TransformCoord(tri1V2, worldMatrix);
		tri1V3 = XMVector3TransformCoord(tri1V3, worldMatrix);

		// Find the normal using U, V coordinates (two edges)
		DirectX::XMVECTOR U = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR V = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		DirectX::XMVECTOR faceNormal = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = DirectX::XMVectorSubtract(tri1V2, tri1V1); 
		V = DirectX::XMVectorSubtract(tri1V3, tri1V1);

		// Compute face normal by crossing U, V
		faceNormal = DirectX::XMVector3Cross(U, V);

		faceNormal = DirectX::XMVector3Normalize(faceNormal);

		// Calculate a point on the triangle for the plane equation
		DirectX::XMVECTOR triPoint = tri1V1;

		// Get plane equation ("Ax + By + Cz + D = 0") Variables
		// (" D = -Ax -By -Cz")
		float tri1A =  DirectX::XMVectorGetX(faceNormal);
		float tri1B =  DirectX::XMVectorGetY(faceNormal);
		float tri1C =  DirectX::XMVectorGetZ(faceNormal);
		float tri1D = (-tri1A * DirectX::XMVectorGetX(triPoint) - tri1B * DirectX::XMVectorGetY(triPoint) - tri1C * DirectX::XMVectorGetZ(triPoint));

		// Now we find where (on the ray) the ray intersects with the triangles plane
		float ep1, ep2, t = 0.0f;
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
		DirectX::XMVECTOR pointInPlane = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		ep1 = (tri1A *  DirectX::XMVectorGetX(m_RayInWorldSpacePos)) + (tri1B *  DirectX::XMVectorGetY(m_RayInWorldSpacePos)) + (tri1C *  DirectX::XMVectorGetZ(m_RayInWorldSpacePos));
		ep2 = (tri1A *  DirectX::XMVectorGetX(m_RayInWorldSpaceDir)) + (tri1B *  DirectX::XMVectorGetY(m_RayInWorldSpaceDir)) + (tri1C *  DirectX::XMVectorGetZ(m_RayInWorldSpaceDir));

		// Make sure there are no divide-by-zeros
		if (ep2 != 0.0f)
			t = -(ep1 + tri1D) / (ep2);

		// Make sure you not to pick objects behind the m_pCamera
		if (t > 0.0f)    
		{
			// Get the point on the plane
			planeIntersectX =  DirectX::XMVectorGetX(m_RayInWorldSpacePos) +  DirectX::XMVectorGetX(m_RayInWorldSpaceDir) * t;
			planeIntersectY =  DirectX::XMVectorGetY(m_RayInWorldSpacePos) +  DirectX::XMVectorGetY(m_RayInWorldSpaceDir) * t;
			planeIntersectZ =  DirectX::XMVectorGetZ(m_RayInWorldSpacePos) +  DirectX::XMVectorGetZ(m_RayInWorldSpaceDir) * t;

			pointInPlane = DirectX::XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			// Call function to check if point is in the triangle
			if (isPointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
			{
				// Return the distance to the hit, so you can check all the other pickable objects in your scene
				// and choose whichever object is closest to the m_pCamera
				distance = t / 2.0f;
				return true;
			}
		}
	}
	return false;
}

bool MousePicker::isPointInTriangle(DirectX::XMVECTOR& triV1, DirectX::XMVECTOR& triV2, DirectX::XMVECTOR& triV3, DirectX::XMVECTOR& point)
{
	// To find out if the point is inside the triangle, we will check to see if the point
	// is on the correct side of each of the triangles edges.

	 DirectX::XMVECTOR cp1 =  DirectX::XMVector3Cross((DirectX::XMVectorSubtract(triV3, triV2)), (DirectX::XMVectorSubtract(point, triV2)));
	 DirectX::XMVECTOR cp2 =  DirectX::XMVector3Cross((DirectX::XMVectorSubtract(triV3, triV2)), (DirectX::XMVectorSubtract(triV1, triV2)));
	if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 =  DirectX::XMVector3Cross((DirectX::XMVectorSubtract(triV3, triV1)), (DirectX::XMVectorSubtract(point, triV1)));
		cp2 =  DirectX::XMVector3Cross((DirectX::XMVectorSubtract(triV3, triV1)), (DirectX::XMVectorSubtract(triV2, triV1)));
		if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 =  DirectX::XMVector3Cross((DirectX::XMVectorSubtract(triV2, triV1)), (DirectX::XMVectorSubtract(point, triV1)));
			cp2 =  DirectX::XMVector3Cross((DirectX::XMVectorSubtract(triV2, triV1)), (DirectX::XMVectorSubtract(triV3, triV1)));
			if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cp1, cp2)) >= 0)
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
