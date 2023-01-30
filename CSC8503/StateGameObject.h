#pragma once
#include "UpdateableObject.h"

namespace NCL {
    namespace CSC8503 {
		class GameWorld;
        class StateMachine;
        class EnemyStateObject : public UpdateableObject {
        public:
			EnemyStateObject(GameWorld* world = nullptr);
            ~EnemyStateObject();

            void Update(float dt) override;


			void OnCollisionBegin(GameObject* otherObject) override;
			void OnCollisionEnd(GameObject* otherObject) override;

        protected:
            void MoveForward(float dt);
            void TurnAround(float dt);
            void RunAway(float dt);

			void InstantTurnAround();

            StateMachine* stateMachine;
            float counter;
			GameWorld* world;

			Vector3 forward;

			bool dead;
        };
    }
}
