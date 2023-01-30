#pragma once
#include "UpdateableObject.h"

#include "NavigationGrid.h"

class BehaviourParalel;

namespace NCL {
    namespace CSC8503 {
		enum DoorKey
		{
			KEY_RED = 0,
			KEY_GREEN = 1,
			KEY_BLUE = 2,

			KEY_NUM = 3
		};


		class GameWorld;
        class DoorObject : public UpdateableObject {
        public:
			DoorObject(DoorKey type);
            ~DoorObject();

			void Update(float dt) override;

			static bool Is_keyUnlocked(DoorKey key)
			{
				return collected_keys[(int)key];
			}

			static void UnlockDoor(DoorKey key)
			{
				collected_keys[(int)key] = true;
			}

		protected:
			DoorKey	keytype;

			static bool  collected_keys[KEY_NUM];
        };
    }
}
