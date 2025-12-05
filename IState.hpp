#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

enum class StateSignal {
	NONE = 0,
	CHANGE_TO_GAMESTATE,
	CHANGE_TO_PAUSESTATE,
	EXIT_APPLICATION
};

class IState {
	public:
		// Default constructor and destructor
		virtual ~IState() = default;
		virtual int OnEnter();
		virtual int OnExit();
		virtual int Update();
		virtual int Render(sf::RenderWindow& window);
};