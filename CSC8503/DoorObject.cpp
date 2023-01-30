#include "DoorObject.h"

using namespace NCL;
using namespace CSC8503;

bool DoorObject::collected_keys[KEY_NUM] = { false };

DoorObject::DoorObject(DoorKey type) : UpdateableObject("Door")
{
	keytype = type;
	SetLayer(GameLayers::GL_DOOR);
}

DoorObject::~DoorObject() {

}

void DoorObject::Update(float dt) 
{
	isActive = !collected_keys[keytype];
}