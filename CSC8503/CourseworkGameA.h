#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif

#include "StateGameObject.h"
#include "BTEnemyObject.h"
#include "PlayerObject.h"
#include "KeyObject.h"
#include "Game.h"

namespace NCL {
	namespace CSC8503 {
		class CourseworkGameA : public Game {
		public:
			CourseworkGameA();
			~CourseworkGameA();
			
			void UpdateGame(float dt) override;
			void InitWorld() override;

			void UpdateRenderer(float dt) override {
				renderer->Update(dt);
				renderer->Render();
				Debug::UpdateRenderables(dt);
			}

		// Methods
		protected:
			void InitialiseAssets() override;

			void InitCamera() override;
			void UpdateKeys() override;
			void InitDestructables();

			void UpdateThirdPerson(float dt);

			void LockedObjectMovement();

			void GrapplePlayer();
			void UnGrapplePlayer();

			void EndGame();

			// Entities
			PlayerObject* AddPlayerToWorld(const Vector3& position);
			EnemyStateObject* AddEnemyToWorld(const Vector3& position);
			BTEnemyObject* AddGooseToWorld(const Vector3& position, Vector3 labyrinth_pos, std::string filename);

			// Immovable Structures
			GameObject* AddWallToWorld(const Vector3& position, Vector3 half_dimensions);
			GameObject* AddDoorToWorld(const Vector3& position, Vector3 half_dimensions, DoorKey keytype);
			GameObject* AddFloorToWorld(const Vector3& position, const Vector3 half_dimensions = Vector3(200, 2, 200));
			GameObject* AddDeadZone(const Vector3& position, Vector3 dimensions);

			// Collectables
			GameObject* AddKeyToWorld(const Vector3& position, Vector3 dimensions, DoorKey keytype);
			GameObject* AddCollectableToWorld(const Vector3& position, float radius);

			// Interactive
			GameObject* AddGrappleToWorld(const Vector3& position, Vector3 half_dimensions, float trigger_radius = 15.0f);

			// Basics
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f, GameLayers layer = GameLayers::GL_WALL);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, GameLayers layer = GameLayers::GL_DESTRUCTABLE);

			// Read from file
			void AddStructureFromFile(const Vector3& position, const std::string filename);
			void AddExtrasFromFile(const Vector3& position, const std::string filename);

		// Fields
		protected:

#ifdef USEVULKAN
			GameTechVulkanRenderer* renderer;
#else
			GameTechRenderer* renderer;
#endif

			MeshGeometry* playerMesh	= nullptr;
			MeshGeometry* cubeMesh		= nullptr;
			MeshGeometry* gooseMesh		= nullptr;
			MeshGeometry* enemyMesh		= nullptr;
			MeshGeometry* bonusMesh		= nullptr;
			MeshGeometry* sphereMesh	= nullptr;

			TextureBase* basicTex		= nullptr;
			ShaderBase*  basicShader	= nullptr;
			

			PlayerObject*		playerObject	= nullptr;
			GameObject*			gooseObject		= nullptr;

			GameObject*			lockedObject	= nullptr;
			Vector3				lockedOffset	= Vector3(0, 3.5f, 13);

			
			std::vector<UpdateableObject* > updateables;
			std::vector<GameObject* > colectables;
		};
	}
}