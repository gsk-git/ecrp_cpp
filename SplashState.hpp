#pragma once

#include "IState.hpp"

class SplashState : public IState {
	public:
		// Default constructor and destructor
		SplashState() = default;
		~SplashState() = default;
		// State member functions
		int OnEnter() override;
		int OnExit() override;
		int Update() override;
		int Render(sf::RenderWindow& window) override;
};

