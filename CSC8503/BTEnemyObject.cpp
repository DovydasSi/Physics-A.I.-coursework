#include "BTEnemyObject.h"

#include "BehaviourNode.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourParalel.h"
#include "BehaviourAction.h"
#include "GameWorld.h"
#include "Window.h"

using namespace NCL;
using namespace CSC8503;

BTEnemyObject::BTEnemyObject(Vector3 pos, std::string filename, GameWorld* w,Vector3 labyrinth_pos) : UpdateableObject("BT Enemy")
{
	world = w;
	grid = new NavigationGrid(filename, labyrinth_pos);
	speed = 0.50f;
	forward = { 0, 0, -1 };
	labPos = labyrinth_pos;
	start_pos = pos;
	respawn = false;
	show_debug = false;

	transform.SetPosition(pos);
	currWaypoint = transform.GetPosition();


	BehaviourAction* pickPath = new BehaviourAction("Pick a path", 
		[&](float dt, BehaviourState state)->BehaviourState
		{
			if (state == Initialise) {
				path.Clear();
				if (this->grid->PathToRandomNode(currWaypoint, path))
				{
					if (show_debug)
					{
						for (int i = 1; i < path.waypoints.size(); ++i) {
							Vector3 a = path.waypoints[i - 1];
							Vector3 b = path.waypoints[i];

							a.y = this->GetTransform().GetPosition().y;
							b.y = this->GetTransform().GetPosition().y;

							Debug::DrawLine(labPos + a, labPos + b, Vector4(1, 1, 1, 1), 15.0f);
						}
					}
					speed = 0.50f;
					return Success;
				}
				else
				{
					return Failure;
				}
			}
			return state;
		}
		);

	BehaviourAction* lookForPlayer = new BehaviourAction("Look for player",
		[&](float dt, BehaviourState state)->BehaviourState
		{
			Vector3 pos = this->GetTransform().GetPosition();
			Ray r = Ray(pos, forward);
			RayCollision rc;
			state = state == Initialise ? Failure : state;

			if (this->world->Raycast(r, rc, true, this) &&
				(((GameObject *) rc.node)->GetLayerMask() & 1 << (int) GameLayers::GL_PLAYER) != 0)
			{
				// Change destination here
				path.Clear();
				if (this->grid->FindPath(currWaypoint, rc.collidedAt, path))
				{
					if (show_debug)
					{
						for (int i = 1; i < path.waypoints.size(); ++i) {
							Vector3 a = path.waypoints[i - 1];
							Vector3 b = path.waypoints[i];

							a.y = this->GetTransform().GetPosition().y;
							b.y = this->GetTransform().GetPosition().y;

							Debug::DrawLine(labPos + a, labPos + b, Vector4(0, 0, 1, 1), 5.0f); // blue path for when they see the player
						}
					}

					state = Success;
					if (path.PopWaypoint(currWaypoint))
					{
						currWaypoint += labPos;
						currWaypoint.y = this->GetTransform().GetPosition().y;
						to_nextWaypoint = currWaypoint - this->GetTransform().GetPosition();
						forward = to_nextWaypoint.Normalised();
					}
					speed = 1.0f;
				}
			}
			return state;
		}
	);

	BehaviourAction* walkPath = new BehaviourAction("Walk to next waypoint",
		[&](float dt, BehaviourState state)->BehaviourState
		{
			if (state == Initialise ) { // if successfully 
				
				if (path.PopWaypoint(currWaypoint))
				{
					currWaypoint += labPos;
					currWaypoint.y = this->GetTransform().GetPosition().y;
					to_nextWaypoint = currWaypoint - this->GetTransform().GetPosition();
					forward = to_nextWaypoint.Normalised();
					return Ongoing;
				}
				return Failure;
			}

			if (state == Ongoing)
			{
				Vector3 pos = this->GetTransform().GetPosition();

				this->GetTransform().SetPosition(pos + to_nextWaypoint * speed * dt);

				pos = this->GetTransform().GetPosition();

				if (show_debug)
				{
					Debug::DrawLine(pos, currWaypoint, Vector4(1, 0, 0, 1), dt);
				}

				if (std::abs((currWaypoint - pos).Length()) < 3)
				//if (this->grid->ClosestNode(this->GetTransform().GetPosition()) == this->grid->ClosestNode(currWaypoint))
				{
					return Initialise; // Reset when reach a waypoint
				}

				return Ongoing;
			}

			return state;
		}
	);

	BehaviourSelector* sl = new BehaviourSelector("Select Destination");

	sl->AddChild(lookForPlayer);
	sl->AddChild(pickPath);

	root = new BehaviourParalel("Paralel root");
	root->AddChild(sl);
	root->AddChild(walkPath);

	root->Reset();

	SetLayer(GameLayers::GL_GOOSE);
}

BTEnemyObject::~BTEnemyObject() {
	delete grid;
	delete root;
}

void BTEnemyObject::Update(float dt) {
	BehaviourState state = root->Execute(dt);

	if (state == Success || state == Failure)
	{
		root->Reset();
	}

	if (respawn)
	{
		transform.SetPosition(start_pos);
		respawn = false;

		root->Reset();
	}

	if (Window::GetMouse()->ButtonPressed(MouseButtons::MIDDLE))
	{
		show_debug = !show_debug;
	}
}

void BTEnemyObject::OnCollisionBegin(GameObject* otherObject) {
	if ((otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_ENEMY)) != 0)
	{
		root->Reset();
	}
	else if ((otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_PLAYER)) != 0)
	{
		root->Reset();
	}
	else if ((otherObject->GetLayerMask() & (1 << (int)GameLayers::GL_DEADZONE)) != 0)
	{
		respawn = true;
	}
}