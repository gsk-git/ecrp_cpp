#include <SFML/Graphics/RenderWindow.hpp>
#include "SplashState.hpp"
#include "IState.hpp"

int SplashState::OnEnter() {
	// Initialization code for the splash state
	return 0; // Return 0 on success
}

int SplashState::OnExit() {
	// Cleanup code for the splash state
	return 0; // Return 0 on success
}

int SplashState::Update() {
	// Update logic for the splash state
	return static_cast<int>(StateSignal::CHANGE_TO_GAMESTATE); // Example: transition to GameState
}

int SplashState::Render(sf::RenderWindow& window) {
	// Rendering code for the splash state
	return 0; // Return 0 on success
}