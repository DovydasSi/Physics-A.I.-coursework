#pragma once
#include "GameObject.h"
#include "GameClient.h"
#include "PositionConstraint.h"
#include "GameWorld.h"
#include <string>

namespace NCL 
{
	namespace CSC8503 
	{
		class PlayerObject : public GameObject 
		{
		public:
			PlayerObject(GameWorld* w, std::string name = "");
			~PlayerObject();

			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherObject) override;

			bool ShouldReset()
			{
				return reset;
			}

			bool CanGrapple() const {
				return can_grapple;
			}

			bool IsGrappled() const {
				return is_grappled;
			}

			GameObject* GetGrappleObject() const {
				return grapple_object;
			}

			void Grapple() {
				is_grappled = true;
			}

			void UnGrapple() {
				constraint = nullptr;
				is_grappled = false;
			}

			void SetConstraint(PositionConstraint* cons)
			{
				constraint = cons;
			}

			PositionConstraint* GetConstraint()
			{
				return constraint;
			}

		protected:
			bool reset;

			bool can_grapple;
			bool is_grappled;
			GameObject* grapple_object;
			PositionConstraint* constraint;

			GameWorld* world;
		};
	}
}