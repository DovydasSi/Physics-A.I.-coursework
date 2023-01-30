#pragma once
#include "GameObject.h"
#include <string>

namespace NCL 
{
	namespace CSC8503 
	{
		class UpdateableObject : public GameObject 
		{
		public:
			UpdateableObject(std::string name = "") : GameObject(name) {}

			virtual void Update(float dt) = 0;
		};
	}
}

