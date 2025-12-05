#include <SFML/Graphics/RenderWindow.hpp>
#include "PauseState.hpp"
#include "IState.hpp"

int PauseState::OnEnter() {
	// Initialization code for the pause state
	return 0; // Return 0 on success
}

int PauseState::OnExit() {
	// Cleanup code for the pause state
	return 0; // Return 0 on success
}

int PauseState::Update() {
	// Update logic for the pause state
	return static_cast<int>(StateSignal::NONE); // Example: stay in PauseState
}

int PauseState::Render(sf::RenderWindow& window) {
	// Rendering code for the pause state
	return 0; // Return 0 on success
}
