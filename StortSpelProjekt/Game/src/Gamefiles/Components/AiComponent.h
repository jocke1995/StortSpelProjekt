#ifndef AICOMPONENT_H
#define AICOMPONENT_H
#include "../ECS/Components/Component.h"
#include "EngineMath.h"
#include "Core.h"
#include <vector>
#include <string>
#include <unordered_map>

class EngineRand;
class Scene;
class NavMesh;
struct NavQuad;

enum F_AI_FLAGS
{
	CAN_JUMP = BIT(1),
	CAN_ROLL = BIT(2),
};

struct PathQuad
{
	std::vector<float3> entranceTilesRight;
	std::vector<float3> entranceTilesLeft;
	std::vector<float3> entranceTilesForward;
	std::vector<float3> entranceTilesBack;
};

struct Tile
{
	float g = 0;
	float f = 0;
	bool closed = false;
	float3 parent;
};

namespace component
{
	class AiComponent : public Component
	{
	public:
		// Default Settings
		AiComponent(Entity* parent, Entity* target, unsigned int flags = 0, float detectionRadius = 25.0f, float attackingDistance = 3.5f);
		virtual ~AiComponent();

		void SetScene(Scene* scene);

		void Update(double dt);
		void RenderUpdate(double dt);

		void OnInitScene();
		void OnLoadScene();
		void OnUnloadScene();

		void AddTarget(Entity* target);
		void RemoveTarget(std::string name);
		Entity* GetTarget();

	private:
		Entity* m_pTarget;
		Scene* m_pScene;
		std::vector<Entity*> m_Targets;
		std::vector<float3> m_Path;
		float m_DetectionRadius;
		float m_AttackingDistance;
		unsigned int m_Flags;
		NavMesh* m_pNavMesh;

		PathQuad* m_pQuads;
		NavQuad* m_pCurrentQuad;
		
		float3 m_StartPos;
		float3 m_GoalPos;
		float3 m_CurrentTile;
		std::vector<float3> m_OpenList;
		std::unordered_map<float2, Tile> m_Tiles;

		void selectTarget();
		void findPathToTarget();

		void checkAdjacent();
		bool moveToNextTile();
	};
}

#endif