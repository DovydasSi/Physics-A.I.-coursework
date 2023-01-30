#pragma once
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class PhysicsSystem	{
		public:
			PhysicsSystem(GameWorld& g);
			~PhysicsSystem();

			void Clear();

			void Update(float dt);

			void UseGravity(bool state) {
				applyGravity = state;
			}

			void SetGlobalDamping(float d) {
				globalDamping = d;
			}
			
			float GetLinearDamping() const {
				return linearDamping;
			}

			void SetLinearDamping(float d) {
				linearDamping = d;
			}

			float GetAngularDamping() const {
				return angularDamping;
			}

			void SetAngularDamping(float d) {
				angularDamping = d;
			}

			void SetGravity(const Vector3& g);
		protected:
			void BasicCollisionDetection();
			void BroadPhase();
			void NarrowPhase();

			void ClearForces();

			void IntegrateAccel(float dt);
			void IntegrateVelocity(float dt);

			void UpdateConstraints(float dt);

			void UpdateCollisionList();
			void UpdateObjectAABBs();

			void ImpulseResolveCollision(GameObject& a , GameObject&b, CollisionDetection::ContactPoint& p) const;

			void LoadSkippedCollisions();
			bool CollisionMaskCheck(GameObject* a, GameObject* b);

			GameWorld& gameWorld;

			bool	applyGravity;
			Vector3 gravity;
			float	dTOffset;
			float	globalDamping;
			float	linearDamping;
			float	angularDamping;

			std::set<CollisionDetection::CollisionInfo> allCollisions;
			std::set<CollisionDetection::CollisionInfo> broadphaseCollisions;
			std::vector<CollisionDetection::CollisionInfo> broadphaseCollisionsVec;
			std::vector< uint32_t > skippedCollisionMasks;
			bool useBroadPhase		= true;
			int numCollisionFrames	= 5;
		};
	}
}

