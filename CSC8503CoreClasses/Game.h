#pragma once
#include "PhysicsSystem.h"
// Base class for games classes like TutorialGame
// Makes it easier to switch games from the main function
// Contains virtual functions that probably should be overriden in all Game instances

namespace NCL {
	namespace CSC8503 {
		enum GameType
		{
			TimeTrial,
			Training
		};

		enum GameState
		{
			GS_Ongoing,
			GS_Ended,
			GS_Paused
		};


		class Game {
		public:
			Game()
			{
				world = new GameWorld();
				physics = new PhysicsSystem(*world);
				paused = false;
				gametype = Training;
			}

			~Game()
			{
				delete world; 
				delete physics;
			}

			virtual void UpdateGame(float dt) = 0;
			virtual void InitWorld() = 0;

			void SetGameType(GameType type)
			{
				this->gametype = type;
			}

			GameType GetGameType() const
			{
				return gametype;
			}

			void SetGameType(GameState state)
			{
				this->gamestate = state;
			}

			GameState GetGameState() const
			{
				return gamestate;
			}

			int GetScore()
			{
				return world->GetScore();
			}

			virtual void UpdateRenderer(float dt) = 0;

			int GetHighscore(std::string playername)
			{
				int res = 0;

				for (auto& pair : highscores)
				{
					if (pair.first == playername && pair.second > res)
					{
						res = pair.second;
					}
				}

				return res;
			}
			
		protected:
			virtual void InitialiseAssets() = 0;

			virtual void InitCamera() = 0;
			virtual void UpdateKeys() = 0;

		protected:
			PhysicsSystem* physics;
			GameWorld* world;

			bool paused;
			float timer;

			GameType gametype;
			GameState gamestate;
			std::vector<std::pair<const char*, int>> highscores;
		};
	}
}