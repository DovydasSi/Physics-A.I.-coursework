#pragma once
#include "UpdateableObject.h"
#include "DoorObject.h"

class BehaviourParalel;

namespace NCL {
    namespace CSC8503 {
		class GameWorld;
        class KeyObject : public UpdateableObject {
        public:

			KeyObject(DoorKey type) : UpdateableObject("Door")
			{
				this->type = type;
				SetLayer(GameLayers::GL_KEY);
			}

			~KeyObject() {}

			DoorKey GetKeyType()
			{
				return type;
			}

			void Update(float dt) override;

			void OnCollisionBegin(GameObject* otherObject) override;

		protected:
			DoorKey type;
        };
    }
}
