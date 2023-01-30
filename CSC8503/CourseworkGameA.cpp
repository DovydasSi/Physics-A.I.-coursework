#include "CourseworkGameA.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include "BTEnemyObject.h"

#include "Maths.h"
#include "Assets.h"
#include <fstream>

using namespace NCL;
using namespace CSC8503;
using namespace Maths;

#define NUM_DESTRUCTABLES	(30)
#define TIMER_SECONDS		(90)

CourseworkGameA::CourseworkGameA() : Game()
{
#ifdef USEVULKAN
	renderer = new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif
	
	InitialiseAssets();
}

CourseworkGameA::~CourseworkGameA()
{
	delete playerMesh;
	delete cubeMesh;
	delete basicTex;
	delete basicShader;
	delete enemyMesh;
	delete bonusMesh;
	delete gooseMesh;
	delete sphereMesh;

	delete renderer;
}

void CourseworkGameA::UpdateGame(float dt)
{
	if (Window::GetMouse()->ButtonPressed(MouseButtons::RIGHT))
	{
		paused = !paused;
	}

	if (paused)
	{
		world->GetMainCamera()->UpdateCamera(dt);
		renderer->Update(dt);
		renderer->Render();
		return;
	}

	UpdateKeys();
	

	if (lockedObject != nullptr)
	{
		Vector3 lo = Matrix4::Rotation(lockedObject->GetTransform().GetOrientation().ToEuler().y, { 0, 1, 0 }) * lockedOffset;

		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		
		Vector3 camPos = objPos + lo;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler();

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x + 10);
		world->GetMainCamera()->SetYaw(angles.y);

		LockedObjectMovement();
	} 
	else
	{
		// Update camera and its controls if we are not locked to the player
		world->GetMainCamera()->UpdateCamera(dt);
	}

	if (playerObject)
	{
		Debug::Print("Score: " + std::to_string(world->GetScore()), { 45, 5 });

		if (playerObject->CanGrapple())
		{
			Debug::Print("[LMB] Grapple", { 45, 90 });
		}

		if (playerObject->ShouldReset() || world->GetScore() == colectables.size() || 
			(gametype == TimeTrial && timer <= 0))
		{
			EndGame();
		}

	}

	if (gametype == TimeTrial)
	{
		timer -= dt;
		Debug::Print(std::to_string((int)floorf(timer) ), { 10, 5 });
	}

	for (UpdateableObject* obj : updateables)
	{
		if (obj)
		{
			obj->Update(dt);
		}
	}
	
	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void CourseworkGameA::InitialiseAssets()
{
	cubeMesh = renderer->LoadMesh("cube.msh");
	playerMesh = renderer->LoadMesh("goat.msh");
	enemyMesh = renderer->LoadMesh("Keeper.msh");
	gooseMesh = renderer->LoadMesh("RamboGoose.msh");
	bonusMesh = renderer->LoadMesh("Key.msh");
	sphereMesh = renderer->LoadMesh("sphere.msh");

	basicTex = renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
}

void CourseworkGameA::InitCamera()
{
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));

	playerObject = nullptr;
}

void CourseworkGameA::InitWorld()
{
	world->ClearAndErase();
	physics->Clear();
	updateables.clear();
	colectables.clear();
	paused = false;

	gamestate = GS_Ongoing;

	// Floors
	AddFloorToWorld({ 0.0f, -20.0f, 0.0f });
	AddFloorToWorld({ 0.0f, -20.0f, -380.0f }, { 200, 2, 120 });
	AddDeadZone({ 0.0f, -50.0f, 0.0f }, { 1000.0f, 1.0f, 1000.0f });

	AddCubeToWorld({ 40.0f, -10.0f, -10.0 }, { 5, 5.0f, 5 }, 1000);


	AddGrappleToWorld({ 40.0f, -10.0f, -225.0 }, { 0.50f, 0.50f, 0.50f }, 20.0f);
	AddGrappleToWorld({ 20.0f, -6.0f, -245.0 }, { 0.50f, 0.50f, 0.50f }, 20.0f);

	Vector3 labyrinthPos = { -200, 0, -200 };
	AddStructureFromFile(labyrinthPos, "GameMaze.txt");

	labyrinthPos = { -100.0f, 0.0f, -400.0f };
	AddStructureFromFile(labyrinthPos, "GameMaze2.txt");

	lockedObject = AddPlayerToWorld({-5.0f, -8.0f, 20.0f});

	AddEnemyToWorld({ 50, 3, 100 });

	timer = TIMER_SECONDS;

	world->GetMainCamera()->SetFieldOfVision(30.0f);

	physics->UseGravity(true);
	physics->SetLinearDamping(0.75f);
	physics->SetAngularDamping(0.75f);
}

void CourseworkGameA::UpdateKeys()
{
	
}

GameObject* CourseworkGameA::AddFloorToWorld(const Vector3& position, const Vector3 half_dimensions)
{
	GameObject* floor = new GameObject("Floor");

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(half_dimensions);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(half_dimensions * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->GetRenderObject()->SetColour({ 0, 1, 0.0f, 1 });

	floor->SetLayer(GameLayers::GL_FLOOR);

	world->AddGameObject(floor);

	return floor;
}

GameObject* CourseworkGameA::AddWallToWorld(const Vector3& position, Vector3 half_dimensions)
{
	GameObject* cube = new GameObject("Cube");

	AABBVolume* volume = new AABBVolume(half_dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(half_dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, nullptr, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->GetRenderObject()->SetColour({ 0, 1, 0.2f, 1 });

	cube->SetLayer(GameLayers::GL_WALL);

	world->AddGameObject(cube);

	return cube;
}

PlayerObject* CourseworkGameA::AddPlayerToWorld(const Vector3& position)
{
	float meshSize = 1.5f;
	float inverseMass = 0.5f;

	PlayerObject* character = new PlayerObject(world, "Goat");
	SphereVolume* volume = new SphereVolume(1.2f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), playerMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	playerObject = character;

	return character;
}

EnemyStateObject* CourseworkGameA::AddEnemyToWorld(const Vector3& position)
{
	float meshSize = 2.0f;
	float inverseMass = 10.0f;

	EnemyStateObject* character = new EnemyStateObject(world);
	SphereVolume* volume = new SphereVolume(1.5f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);
	updateables.push_back(character);

	return character;
}

BTEnemyObject* CourseworkGameA::AddGooseToWorld(const Vector3& position, Vector3 labyrinth_pos, std::string filename)
{
	float meshSize = 1.5f;
	float inverseMass = 5.0f;

	BTEnemyObject* character = new BTEnemyObject(position, filename, world, labyrinth_pos);
	SphereVolume* volume = new SphereVolume(meshSize * 1.23f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize));

	character->SetRenderObject(new RenderObject(&character->GetTransform(), gooseMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);
	updateables.push_back(character);

	return character;
}

GameObject* CourseworkGameA::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, GameLayers layer) {
	GameObject* cube = new GameObject("Cube");

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->SetLayer(layer);

	world->AddGameObject(cube);

	return cube;
}

GameObject* CourseworkGameA::AddSphereToWorld(const Vector3& position, float radius, float inverseMass, GameLayers layer) {
	GameObject* sphere = new GameObject("Sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	sphere->SetLayer(layer);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* CourseworkGameA::AddCollectableToWorld(const Vector3& position, float radius)
{
	GameObject* sphere = new GameObject("Collectible");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	volume->is_trigger = true;
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(0);
	sphere->GetPhysicsObject()->InitSphereInertia();

	sphere->SetLayer(GameLayers::GL_COLLECTABLE);

	world->AddGameObject(sphere);
	colectables.push_back(sphere);

	return sphere;
}

GameObject* CourseworkGameA::AddDoorToWorld(const Vector3& position, Vector3 half_dimensions, DoorKey keytype)
{
	UpdateableObject* cube = new DoorObject(keytype);

	AABBVolume* volume = new AABBVolume(half_dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(half_dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, nullptr, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->GetRenderObject()->SetColour(
		{
			(keytype == KEY_RED ? 1.0f : 0.0f), 
			(keytype == KEY_GREEN ? 1.0f : 0.0f), 
			(keytype == KEY_BLUE ? 1.0f : 0.0f), 
			1.0f
		});

	world->AddGameObject(cube);
	updateables.push_back(cube);

	return cube;
}

GameObject* CourseworkGameA::AddKeyToWorld(const Vector3& position, Vector3 dimensions, DoorKey keytype) 
{
	UpdateableObject* key = new KeyObject(keytype);

	AABBVolume* volume = new AABBVolume(dimensions);
	key->SetBoundingVolume((CollisionVolume*)volume);
	volume->is_trigger = true;

	key->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions );

	key->SetRenderObject(new RenderObject(&key->GetTransform(), bonusMesh, nullptr, basicShader));
	key->SetPhysicsObject(new PhysicsObject(&key->GetTransform(), key->GetBoundingVolume()));

	key->GetPhysicsObject()->SetInverseMass(0);
	key->GetPhysicsObject()->InitCubeInertia();

	switch (keytype)
	{
	case KEY_RED: key->GetRenderObject()->SetColour({ 1.0f, 0.0f, 0.0f, 1.0f }); break;
	case KEY_GREEN: key->GetRenderObject()->SetColour({ 0.0f, 1.0f, 0.0f, 1.0f }); break;
	case KEY_BLUE: key->GetRenderObject()->SetColour({ 0.0f, 0.0f, 1.0f, 1.0f }); break;
	default: key->GetRenderObject()->SetColour({ 1.0f, 1.0f, 1.0f, 1.0f }); break;
	}


	world->AddGameObject(key);
	updateables.push_back(key);

	return key;
	
}

GameObject* CourseworkGameA::AddDeadZone(const Vector3& position, Vector3 dimensions)
{
	GameObject* dzone = new GameObject("DeadZone");

	AABBVolume* volume = new AABBVolume(dimensions);
	dzone->SetBoundingVolume((CollisionVolume*)volume);
	dzone->GetTransform()
		.SetScale(dimensions * 2)
		.SetPosition(position);

	dzone->SetPhysicsObject(new PhysicsObject(&dzone->GetTransform(), dzone->GetBoundingVolume()));

	dzone->GetPhysicsObject()->SetInverseMass(0);
	dzone->GetPhysicsObject()->InitCubeInertia();

	dzone->SetLayer(GameLayers::GL_DEADZONE);

	world->AddGameObject(dzone);

	return dzone;
}

GameObject* CourseworkGameA::AddGrappleToWorld(const Vector3& position, Vector3 half_dimensions, float trigger_radius)
{
	GameObject* cube = new GameObject("Grapple");

	SphereVolume* volume = new SphereVolume(trigger_radius);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(half_dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, nullptr, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(0);
	cube->GetPhysicsObject()->InitCubeInertia();

	cube->GetRenderObject()->SetColour({1.0f, 0.0f, 0.0f, 1.0f});

	cube->SetLayer(GameLayers::GL_GRAPPLE);

	volume->is_trigger = true;

	world->AddGameObject(cube);

	return cube;
}

void CourseworkGameA::AddStructureFromFile(const Vector3& position, const std::string filename)
{
	std::ifstream infile(Assets::DATADIR + filename);
	float nodeSize, gridWidth, gridHeight;

	infile >> nodeSize;
	infile >> gridWidth;
	infile >> gridHeight;

	// Trying to fuse horizontal walls into one to save on creating too many GameObjects
	float wall_counter = 1.0f;
	Vector3 old_pos;

	for (int y = 0; y < gridHeight; ++y) {
		for (int x = 0; x < gridWidth; ++x) {

			char type = 0;
			infile >> type;
			Vector3 pos = Vector3((float)(x * nodeSize), -13, (float)(y * nodeSize));

			if (wall_counter > 1)
			{
				pos = old_pos;
				pos.x += (nodeSize * (wall_counter - 1)) / 2.0f;
			}
			else
			{
				old_pos = pos;
			}

			if (type == '.')
			{
				wall_counter = 1.0f;
				continue;
			}
			else if (infile.peek() == type)
			{
				wall_counter++;
				continue;
			}
			else if (type == 'x')
			{
				// Wall
				AddWallToWorld( position + pos, { (nodeSize * wall_counter) / 2.0f, 5, nodeSize / 2.0f } );

			}
			else if (type == 'R' || type == 'G' || type == 'B')
			{
				// Key
				switch (type)
				{
				case 'R': AddDoorToWorld(position + pos, { (nodeSize * wall_counter) / 2.0f, 5, nodeSize / 2.0f }, KEY_RED); break;
				case 'G': AddDoorToWorld(position + pos, { (nodeSize * wall_counter) / 2.0f, 5, nodeSize / 2.0f }, KEY_GREEN); break;
				case 'B': AddDoorToWorld(position + pos, { (nodeSize * wall_counter) / 2.0f, 5, nodeSize / 2.0f }, KEY_BLUE); break;
				default: break;
				}
			}

			wall_counter = 1; // reset after building walls
		}
	}

	infile.close();


	AddExtrasFromFile(position, filename);
}

void CourseworkGameA::AddExtrasFromFile(const Vector3& position, const std::string filename)
{
std::ifstream infile(Assets::DATADIR + filename);
float nodeSize, gridWidth, gridHeight;

infile >> nodeSize;
infile >> gridWidth;
infile >> gridHeight;

for (int y = 0; y < gridHeight; ++y) {
	for (int x = 0; x < gridWidth; ++x) {

		char type = 0;
		infile >> type;
		Vector3 pos = Vector3((float)(x * nodeSize), -13, (float)(y * nodeSize));
		Vector3 keypos = position + pos;
		keypos.y -= 3.5;

		switch (type)
		{
		case 'U': AddGooseToWorld(position + pos, position, filename); break;				// Goose (Behaviour Tree AI + Pathfinding)
		case 'E': AddEnemyToWorld(position + pos); break;									// State AI Enemy
		case 'c': AddCollectableToWorld(keypos, 1.0f); break;								// Collectable
		case 'r': AddKeyToWorld(keypos, { 0.4f,0.2f, 0.05f }, KEY_RED); break;							// Keys
		case 'g': AddKeyToWorld(keypos, { 0.4f,0.2f, 0.05f }, KEY_GREEN); break;
		case 'b': AddKeyToWorld(keypos, { 0.4f,0.2f, 0.05f }, KEY_BLUE); break;
		default: continue;
		}
	}
}

infile.close();
}

void CourseworkGameA::LockedObjectMovement() {
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * 50);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * 50);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		lockedObject->GetPhysicsObject()->AddTorque({0, 5, 0});
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		lockedObject->GetPhysicsObject()->AddTorque({ 0, -5, 0 });
	}

	//
	if (playerObject->CanGrapple() && !playerObject->IsGrappled() && Window::GetMouse()->ButtonHeld(MouseButtons::LEFT))
	{
		GrapplePlayer();
	}
	else if (playerObject->IsGrappled() && !Window::GetMouse()->ButtonHeld(MouseButtons::LEFT))
	{
		UnGrapplePlayer();
	}
}

void CourseworkGameA::GrapplePlayer()
{
	PositionConstraint* constraint = new PositionConstraint(playerObject, playerObject->GetGrappleObject(), 10.0f);
	world->AddConstraint(constraint);
	playerObject->SetConstraint(constraint);
	playerObject->Grapple();

	Debug::DrawLine(playerObject->GetTransform().GetPosition(), playerObject->GetGrappleObject()->GetTransform().GetPosition(), {1.0f, 0.0f, 0.0f, 1.0f}, 3.0f);
}

void CourseworkGameA::UnGrapplePlayer()
{
	world->RemoveConstraint(playerObject->GetConstraint(), true);
	playerObject->UnGrapple();
}

void CourseworkGameA::EndGame()
{
	highscores.emplace_back("Player1", world->GetScore());
	gamestate = GS_Ended;
}
