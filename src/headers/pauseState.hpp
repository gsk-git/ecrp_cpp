#pragma once
#include "IState.hpp"
#include <iostream>

class pauseState : public IState {
public:
	// Member variables
	sf::RectangleShape rectBox;
	pauseState(sf::RenderWindow& windowHandle) : IState(windowHandle) {};

	// Member functions
	void init() override {
		std::cout << "Hello from pause state \n";
		rectBox.setSize({ 800.f,80.f });
		rectBox.setFillColor(sf::Color::Cyan);
		rectBox.setPosition(sf::Vector2f({ 80.f,80.f }));
	};
	void handleEvent(const sf::Event& event) override {
		if (const auto* key = event.getIf<sf::Event::KeyReleased>()) { 
			if (key->code == sf::Keyboard::Key::Escape) m_closePause = true; 
		}
	};
	void update(float dt) override {};
	void render(sf::RenderWindow& window) override {
		window.draw(rectBox);
	};
	void onEnter() override {
		m_closePause = false;
	}
	void onExit() override {}
	bool updateBelow() override { return 0; };
	bool renderBelow() override { return 0; };
	bool shouldCloseApp() const override { return m_closeApp; }
	bool shouldClosePause() const override { return m_closePause; }
	stateType getStateType() const override
	{
		return stateType::PauseMenu;
	}
};