#pragma once

#include "PushdownMachine.h"
#include "PushdownState.h"
#include "CourseworkGameA.h"
#include "Window.h"

class BehaviourParalel;

namespace NCL {
	namespace CSC8503 {

		class GameScoreScreen : public PushdownState
		{
		public:
			GameScoreScreen(Game* g) : game(g) {}

			PushdownResult OnUpdate(float dt, PushdownState** pushFunc)
			{
				Debug::Print("Your HighScore is: " + std::to_string(game->GetHighscore("Player1")), {25, 45});
				Debug::Print("[Space] or [Esc] to quit the game.", { 25, 65 });

				game->UpdateRenderer(dt);

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
				{
					return PushdownResult::Reset;
				}
				else if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
				{
					return PushdownResult::Reset;
				}

				return NoChange;
			}

			void OnAwake() override
			{
				std::cout << "Your HighScore is: " + std::to_string(game->GetHighscore("Player1")) << std::endl;
				std::cout << "[Space] or [Esc] to quit the game." << std::endl;
			}

		protected:
			Game* game;
		};

		class GameEndScreen : public PushdownState
		{
		public:
			GameEndScreen(Game* g) :game(g) {}

			PushdownResult OnUpdate(float dt, PushdownState** pushFunc)
			{
				Debug::Print("GAME OVER! Your Score: " + std::to_string(game->GetScore()), { 25, 45 });
				Debug::Print("Press [Space] to view highscores", { 25, 55 });
				Debug::Print("[Esc] to quit the game.", { 25, 65 });

				game->UpdateRenderer(dt);

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
				{
					*pushFunc = new GameScoreScreen(game);
					return PushdownResult::Push;
				}
				else if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
				{
					return PushdownResult::Reset;
				}

				return NoChange;
			}

			void OnAwake() override
			{
				std::cout << "GAME OVER! Your Score: " + game->GetScore() << std::endl;
				std::cout << "Press [Space] to view highscores" << std::endl;
				std::cout << "[Esc] to quit the game." << std::endl;
			}

		protected:
			Game* game;
		};

		class GamePauseScreen : public PushdownState
		{
		public:
			GamePauseScreen(Game* g) : game(g) {}

			PushdownResult OnUpdate(float dt, PushdownState** pushFunc)
			{
				
				Debug::Print("The game is paused. ", { 25, 45 });
				Debug::Print("Press [Space] to resume", { 25, 55 });
				Debug::Print("[Esc] to quit the game.", { 25, 65 });

				game->UpdateRenderer(dt);

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
				{
					return PushdownResult::Pop;
				}
				else if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
				{
					return PushdownResult::Reset; // Even more?
				}

				return NoChange;
			}

			void OnAwake() override
			{
				std::cout.flush();
				std::cout << "The game is paused. " << std::endl;
				std::cout << "Press [Space] to resume" << std::endl;
				std::cout << "[Esc] to quit the game." << std::endl;
			}

		protected:
			Game* game;
		};

		class GameplayScreen : public PushdownState
		{
		public:
			GameplayScreen(Game* g) : game(g) {}

			PushdownResult OnUpdate(float dt, PushdownState** pushFunc)
			{
				game->UpdateGame(dt);

				// Check game states

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
				{
					*pushFunc = new GamePauseScreen(game);
					return Push;
				}

				if (game->GetGameState() == GS_Ended)
				{
					*pushFunc = new GameEndScreen(game);
					return Push;
				}

				return NoChange;
			}

			void OnAwake() override
			{
			}

		protected:
			Game* game;
		};

		class GameSelectScreen : public PushdownState
		{
		public:
			GameSelectScreen(Game* g) : game(g) {}

			PushdownResult OnUpdate(float dt, PushdownState** pushFunc)
			{
				Debug::Print("Please Select a game mode: ", {25, 45});
				Debug::Print("[1] Time Trial", { 25, 55 });
				Debug::Print("[2] Training", { 25, 65 });
				Debug::Print("[Esc] to go back. ", { 25, 75 });

				game->UpdateRenderer(dt);

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1) || Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD1))
				{
					*pushFunc = new GameplayScreen(game);
					game->SetGameType(TimeTrial);
					game->InitWorld();
					return PushdownResult::Push;
				}
				else if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2) || Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD2))
				{
					*pushFunc = new GameplayScreen(game);
					game->SetGameType(Training);
					game->InitWorld();
					return PushdownResult::Push;
				}
				else if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
				{
					return PushdownResult::Pop;
				}

				return NoChange;
			}

			void OnAwake() override
			{
				std::cout << "Please Select the game mode: " << std::endl;
				std::cout << "[1] Time Trial." << std::endl;
				std::cout << "[2] Training." << std::endl;
				std::cout << "[Esc] to go back. " << std::endl;
			}

		protected:
			Game* game;
		};

		class GameIntroScreen : public PushdownState
		{
		public:
			GameIntroScreen(Game* g) : game(g) {}

			PushdownResult OnUpdate(float dt, PushdownState** pushFunc)
			{
				Debug::Print("Welcome to the game! ", { 35, 45 });
				Debug::Print("[Space] to begin", { 25, 55 });
				Debug::Print("[Esc] to exit.", { 25, 65 });

				game->UpdateRenderer(dt);

				if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
				{
					*pushFunc = new GameSelectScreen(game);
					return PushdownResult::Push;
				}
				else if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
				{
					return PushdownResult::Pop;
				}

				return NoChange;
			}

			void OnAwake() override
			{
				std::cout << "Welcome to the game!" << std::endl;
				std::cout << "Press[Space] to begin" << std::endl;
				std::cout << "[Esc] to exit." << std::endl;
			}

		protected:
			Game* game;
		};
	}
}


