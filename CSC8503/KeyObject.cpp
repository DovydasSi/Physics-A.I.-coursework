#include "KeyObject.h"

using namespace NCL;
using namespace CSC8503;

void KeyObject::OnCollisionBegin(GameObject* otherObject) {
	if ((otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_PLAYER)) != 0)
	{
		isActive = false;

		DoorObject::UnlockDoor(type);
	}
}

void KeyObject::Update(float dt) 
{
	isActive = !DoorObject::Is_keyUnlocked(type);
}