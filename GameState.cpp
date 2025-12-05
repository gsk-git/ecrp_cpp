#include <SFML/Graphics/RenderWindow.hpp>
#include "GameState.hpp"
#include "IState.hpp"

int GameState::OnEnter() {
	// Code to initialize game state
	return 0; // Return 0 on success
}

int GameState::OnExit() {
	// Code to clean up game state
	return 0; // Return 0 on success
}

int GameState::Update() {
	// Code to update game state
	return static_cast<int>(StateSignal::NONE); // Return appropriate StateSignal
}

int GameState::Render(sf::RenderWindow& window) {
	// Code to render game state
	return 0; // Return 0 on success
}