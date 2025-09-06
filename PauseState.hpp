#pragma once

#include "IState.hpp"

class PauseState : public IState 
{
	public:
		// Default constructor and destructor
		PauseState() = default;
		~PauseState() = default;
		// State member functions
		int OnEnter() override;
		int OnExit() override;
		int Update() override;
		int Render(sf::RenderWindow& window) override;
};

