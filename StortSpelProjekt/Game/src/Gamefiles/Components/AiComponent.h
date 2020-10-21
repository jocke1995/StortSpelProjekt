#ifndef AICOMPONENT_H
#define AICOMPONENT_H
#include "../ECS/Components/Component.h"
#include "EngineMath.h"
#include "Core.h"
#include <vector>
#include <string>
#include <unordered_map>
#include "../Misc/Multithreading/MultiThreadedTask.h"

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
	int id;
	float g = 0;
	float f = 0;
	bool closed = false;
	PathQuad* parent;
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
	class AiComponent : public Component, public MultiThreadedTask
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

		void Execute();

	private:
		Entity* m_pTarget;
		Scene* m_pScene;
		NavMesh* m_pNavMesh;
		NavQuad* m_pCurrentQuad;
		NavQuad* m_pStartQuad;
		NavQuad* m_pGoalQuad;
		PathQuad** m_pQuads;
		std::unordered_map<std::string, Tile> m_Tiles;
		std::vector<Entity*> m_Targets;
		std::vector<float3> m_Path;
		std::vector<float3> m_NextPath;
		std::vector<int> m_OpenList;
		float3 m_StartPos;
		float3 m_GoalPos;
		float3 m_CurrentTile;
		float3 m_NextTargetPos;
		float m_DetectionRadius;
		float m_AttackingDistance;
		unsigned int m_Flags;
		bool m_PathFound;

		void selectTarget();
		void findPathToTarget();

		void checkAdjacent();
		bool moveToNextTile();
	};
}

#endif