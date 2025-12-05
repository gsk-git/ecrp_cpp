#include "StateManager.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include "IState.hpp"
#include <utility>

StateManager::~StateManager() {
}

void StateManager::ChangeState(std::unique_ptr<IState> nextState) {
}

void StateManager::PushState(std::unique_ptr<IState> nextState) {

}

void  StateManager::PopState() {
}

void StateManager::UpdateState() {
	// Update logic for the current state	
}

void StateManager::RenderState(sf::RenderWindow& window) {
	// Render logic for the current state	
}