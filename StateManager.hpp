#pragma once
#include <vector>
#include <memory>
#include "IState.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

class StateManager {
public:
	// Default constructor
    StateManager() = default;
	// Destructor
    ~StateManager();
    // Replace entire stack with nextState (deferred)
    void ChangeState(std::unique_ptr<IState> nextState);
    // Push and Pop (deferred)
    void PushState(std::unique_ptr<IState> nextState);
    void PopState();
    // Called every frame from main loop
    void UpdateState();
    // Render all states (bottom -> top)
    void RenderState(sf::RenderWindow& window);
private:
    std::vector<std::unique_ptr<IState>> m_states;
};
