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
struct NavTriangle;

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

struct PathTriangle
{
	int id;
	float g = 0;
	float f = 0;
	bool closed = false;
	PathTriangle* parent;
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
		void OnUnInitScene();

		void AddTarget(Entity* target);
		void RemoveTarget(std::string name);
		Entity* GetTarget();

		void SetAttackInterval(float interval);
		void SetMeleeAttackDmg(float dmg);
		void SetRangedAI();

	private:
		Entity* m_pTarget;
		Scene* m_pScene;
		NavMesh* m_pNavMesh;
		NavQuad* m_pCurrentQuad;
		NavQuad* m_pStartQuad;
		NavQuad* m_pGoalQuad;
		NavQuad* m_pNextQuad;
		NavTriangle* m_pCurrentTriangle;
		NavTriangle* m_pStartTriangle;
		NavTriangle* m_pGoalTriangle;
		NavTriangle* m_pNextTriangle;
		PathQuad** m_pQuads;
		PathTriangle** m_pTriangles;
		std::vector<Entity*> m_Targets;
		std::vector<float3> m_Path;
		std::vector<float3> m_NextPath;
		std::vector<int> m_OpenList;
		float3 m_StartPos;
		float3 m_GoalPos;
		float3 m_NextTargetPos;
		float3 m_LastPos;
		float m_DetectionRadius;
		float m_AttackingDistance;
		float m_MeleeAttackDmg;
		unsigned int m_Flags;
		bool m_CanJump;
		float m_AttackInterval;
		float m_TimeAccumulator;
		bool m_PathFound;
		bool m_isRanged;

		void setAimDirection();

		void selectTarget();
		void findPathToTargetQuad();
		void findPathToTargetTriangle();

		void checkAdjacentQuad();
		void checkAdjacentTriangle();
		bool moveToNextQuad();
		bool moveToNextTriangle();

		bool checkIntersectQuad(float2 point1, float2 point2, float2 topLeft, float2 topRight, float2 bottomLeft, float2 bottomRight);
		bool checkIntersectTriangle(float2 point1, float2 point2, float2 vertex1, float2 vertex2, float2 vertex3);
		float lineFunction(float2 point, float2 linePoint1, float2 linePoint2);
	};
}

#endif