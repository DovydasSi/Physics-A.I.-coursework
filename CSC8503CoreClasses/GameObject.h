#pragma once
#include "Transform.h"
#include "CollisionVolume.h"
#include <vector>

using std::vector;

namespace NCL::CSC8503 {

	enum class GameLayers
	{
		GL_PLAYER			= 0,
		GL_COLLECTABLE		= 1,
		GL_DESTRUCTABLE		= 2,
		GL_WALL				= 3,
		GL_FLOOR			= 4,
		GL_ENEMY			= 5,
		GL_DEADZONE			= 6,
		GL_GRAPPLE			= 7,
		GL_KEY				= 8,
		GL_DOOR				= 9,
		GL_GOOSE			= 10,

		GL_DEFAULT
	};

	class NetworkObject;
	class RenderObject;
	class PhysicsObject;

	class GameObject	{
	public:
		GameObject(std::string name = "");
		~GameObject();

		void SetBoundingVolume(CollisionVolume* vol) {
			boundingVolume = vol;
		}

		const CollisionVolume* GetBoundingVolume() const {
			return boundingVolume;
		}

		bool IsActive() const {
			return isActive;
		}

		Transform& GetTransform() {
			return transform;
		}

		RenderObject* GetRenderObject() const {
			return renderObject;
		}

		PhysicsObject* GetPhysicsObject() const {
			return physicsObject;
		}

		NetworkObject* GetNetworkObject() const {
			return networkObject;
		}

		void SetRenderObject(RenderObject* newObject) {
			renderObject = newObject;
		}

		void SetPhysicsObject(PhysicsObject* newObject) {
			physicsObject = newObject;
		}

		const std::string& GetName() const {
			return name;
		}

		virtual void OnCollisionBegin(GameObject* otherObject);

		virtual void OnCollisionEnd(GameObject* otherObject);

		bool GetBroadphaseAABB(Vector3&outsize) const;

		void UpdateBroadphaseAABB();

		void SetWorldID(int newID) {
			worldID = newID;
		}

		int		GetWorldID() const {
			return worldID;
		}

		void SetLayer(GameLayers newLayer) {
			layer = 1 << (int) newLayer;
		}

		uint32_t GetLayerMask() const {
			return layer;
		}

		void SetTag(std::string newTag) {
			tag = newTag;
		}

		std::string GetTag() const {
			return tag;
		}

	protected:
		Transform			transform;

		CollisionVolume*	boundingVolume;
		PhysicsObject*		physicsObject;
		RenderObject*		renderObject;
		NetworkObject*		networkObject;

		bool		isActive;
		int			worldID;
		std::string	name;

		Vector3 broadphaseAABB;

		uint32_t layer;
		std::string tag;
	};
}