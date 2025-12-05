#include "IState.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

int IState::OnEnter() {
	// Logic for entering the state
	return 0; // Return an appropriate status code
}
int IState::OnExit() {
	// Logic for exiting the state
	return 0; // Return an appropriate status code
}
int IState::Update() {
	// Logic for updating the state
	return static_cast<int>(StateSignal::NONE);
}
int IState::Render(sf::RenderWindow& window) {
	// Logic for rendering the state
	(void)window;
	return 0;
}