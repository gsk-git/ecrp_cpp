#pragma once

#include "IState.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

class StateManager {
	public:
		StateManager() = default;
		~StateManager() = default;
		void ChangeState(std::unique_ptr<IState> _current);
		void UpdateState();
		void RenderState(sf::RenderWindow& window);
};

