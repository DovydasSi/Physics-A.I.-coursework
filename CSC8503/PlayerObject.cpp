#include "PlayerObject.h"
#include "KeyObject.h"

using namespace NCL;
using namespace CSC8503;

PlayerObject::PlayerObject(GameWorld* w, std::string name) : GameObject(name)
{
	layer = 1 << (int) GameLayers::GL_PLAYER;
	reset = false;
	can_grapple = false;
	is_grappled = false;
	grapple_object = nullptr;
	constraint = nullptr;
	world = w;
}

PlayerObject::~PlayerObject()	{

}

void PlayerObject::OnCollisionBegin(GameObject* otherObject) {
	if ((otherObject->GetLayerMask() & (1 << (int) GameLayers::GL_COLLECTABLE)) != 0)
	{
		world->AddScore(1);
	}

	if ((otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_DEADZONE)) != 0
		|| (otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_GOOSE)) != 0)
	{
		reset = true;
	}

	if ((otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_GRAPPLE)) != 0)
	{
		can_grapple = true;
		grapple_object = otherObject;
	}
}

void PlayerObject::OnCollisionEnd(GameObject* otherObject) {
	if ((otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_GRAPPLE)) != 0)
	{
		can_grapple = false;
		grapple_object = nullptr;
	}
}

