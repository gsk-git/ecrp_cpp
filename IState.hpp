#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

class IState {
	public:
		// Default constructor and destructor
		virtual ~IState() = default;
		// State member variables
		// State member functions
		virtual int OnEnter();
		virtual int OnExit();
		virtual int Update();
		virtual int Render(sf::RenderWindow& window);
};