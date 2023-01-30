#pragma once
#include "UpdateableObject.h"

#include "NavigationGrid.h"

class BehaviourParalel;

namespace NCL {
    namespace CSC8503 {
		class GameWorld;
        class BTEnemyObject : public UpdateableObject {
        public:
			BTEnemyObject(Vector3 pos, std::string filename, GameWorld* w, Vector3 labyrinth_pos = { 0, 0, 0 });
            ~BTEnemyObject();

			void Update(float dt) override;

			void OnCollisionBegin(GameObject* otherObject) override;

		protected:
		
			NavigationGrid* grid;
			NavigationPath path;

			BehaviourParalel* root;

			GameWorld* world;

			Vector3 forward;
			Vector3 to_nextWaypoint;
			Vector3 currWaypoint;
			Vector3 labPos;

			float speed;

			bool respawn;
			Vector3 start_pos;

			bool show_debug;
        };
    }
}
