#include "GameObject.h"
#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName)	{
	name			= objectName;
	worldID			= -1;
	isActive		= true;
	boundingVolume	= nullptr;
	physicsObject	= nullptr;
	renderObject	= nullptr;
	networkObject	= nullptr;
	layer			= 1 << (int) GameLayers::GL_DEFAULT;
}

GameObject::~GameObject()	{
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
}

void GameObject::OnCollisionBegin(GameObject* otherObject)
{
	if (layer == 1 << (int)GameLayers::GL_COLLECTABLE || layer == 1 << (int)GameLayers::GL_DESTRUCTABLE)
	{
		if (otherObject->GetLayerMask() == 1 << (int)GameLayers::GL_PLAYER ||
			otherObject->GetLayerMask() == 1 << (int)GameLayers::GL_ENEMY)
		{
			isActive = false;
		}
	}

	// deadzone kills all
	if (otherObject->GetLayerMask() == 1 << (int)GameLayers::GL_DEADZONE)
	{
		isActive = false;
	}
}

void GameObject::OnCollisionEnd(GameObject* otherObject)
{

}