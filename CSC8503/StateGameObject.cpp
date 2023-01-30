#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"
#include "GameWorld.h"

using namespace NCL;
using namespace CSC8503;

EnemyStateObject::EnemyStateObject(GameWorld* w) : UpdateableObject("Enemy of the State")
{
	counter = 0.0f;
	stateMachine = new StateMachine();
	world = w;
	layer = 1 << (int) GameLayers::GL_ENEMY;
	dead = false;
	forward = { 0, 0, -1 };
	
	State* moveForward = new State([&](float dt)-> void
		{
			this->MoveForward(dt);
		}
	);
	State * turnAround = new State([&](float dt)-> void
		{
			this->TurnAround(dt);
		}
	);

	State* runAway = new State([&](float dt)-> void
		{
			this->RunAway(dt);
		}
	);
	
	stateMachine->AddState(moveForward);
	stateMachine->AddState(turnAround);
	stateMachine->AddState(runAway);
	
	stateMachine->AddTransition(new StateTransition(moveForward, turnAround,[&]()-> bool
		{
			if (this->counter > 10.0f)
			{
				counter = 0;
				Debug::DrawLine(transform.GetPosition(), transform.GetPosition() + forward, { 1, 0, 1, 1 }, 0.5f);
				return true;
			}
			return false;
		}
	));
	
	stateMachine->AddTransition(new StateTransition(turnAround, moveForward,[&]()-> bool
		{
			if (this->counter > 1.0f)
			{
				counter = 0;
				return true;
			}
			return false;
		}
	));

	StateTransitionFunction func = [&]()-> bool
	{
		Ray r = Ray(GetTransform().GetPosition(), forward);
		RayCollision rc;
		world->Raycast(r, rc, true, this);

		if (rc.rayDistance < 40 && ( ((GameObject*) rc.node)->GetLayerMask() & (1 << (int) GameLayers::GL_PLAYER)) != 0)
		{
			InstantTurnAround();
			return true;
		}
		return false;
	};

	stateMachine->AddTransition(new StateTransition(moveForward, runAway, func));

	stateMachine->AddTransition(new StateTransition(turnAround, runAway, func));

	stateMachine->AddTransition(new StateTransition(runAway, moveForward, [&]()-> bool
		{
			if (this->counter > 5.0f)
			{
				counter = 0;
				return true;
			}
			return false;
		}
	));
}

EnemyStateObject::~EnemyStateObject() {
	delete stateMachine;
}

void EnemyStateObject::Update(float dt) {
	// Rather than adding another state transition to every other state, just stop updating the state machine
	if (!dead) 
	{
		stateMachine->Update(dt);
	}
}

void EnemyStateObject::MoveForward(float dt) {
	GetPhysicsObject()->AddForce(forward * (100 * dt));
	counter += dt;
}

void EnemyStateObject::TurnAround(float dt) {
	Vector3 angles = transform.GetOrientation().ToEuler();

	angles.y += 180 * dt;
	
	if (angles.y > 360)
	{
		angles.y -= 360;
	}

	transform.SetOrientation(Quaternion::EulerAnglesToQuaternion(angles.x, angles.y, angles.z));

	forward = Matrix4::Rotation(180 * dt, { 0, 1, 0 }) * forward;
	counter += dt;
}

void EnemyStateObject::InstantTurnAround() {
	TurnAround(1.0f);
}

void EnemyStateObject::RunAway(float dt) {
	GetPhysicsObject()->AddForce(forward * 200 * dt);
	counter += dt;
}

void EnemyStateObject::OnCollisionBegin(GameObject* otherObject) 
{
	if((otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_DEADZONE)) != 0 )
	{
		dead = true;
		isActive = false;
	}

	if ((otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_PLAYER)) != 0)
	{
		dead = true;
	}
	
	if ((otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_COLLECTABLE)) != 0)
	{
		world->AddScore(1);
	}
}

void EnemyStateObject::OnCollisionEnd(GameObject* otherObject)
{
	GameObject::OnCollisionEnd(otherObject);
}