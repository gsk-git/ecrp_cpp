#pragma once

#include "IState.hpp"

class GameState : public IState {
	public:
		// Default constructor and destructor
		GameState() = default;
		~GameState() = default;
		// State member functions
		int OnEnter() override;
		int OnExit() override;
		int Update() override;
		int Render(sf::RenderWindow& window) override;
};

