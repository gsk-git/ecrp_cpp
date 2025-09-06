#pragma once

#include "IState.hpp"

class MainMenuState : public IState {
	public:
		// Default constructor and destructor
		MainMenuState() = default;
		~MainMenuState() = default;
		// State member functions
		int OnEnter() override;
		int OnExit() override;
		int Update() override;
		int Render(sf::RenderWindow& window) override;
};

