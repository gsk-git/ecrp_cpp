#include "StateManager.hpp"

void StateManager::ChangeState(std::unique_ptr<IState> _current) {
	// Check if the new state is different from the current state	
}

void StateManager::UpdateState() {
	// Update logic for the current state	
}

void StateManager::RenderState(sf::RenderWindow& window) {
	// Render logic for the current state	
}