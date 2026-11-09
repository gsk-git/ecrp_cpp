#pragma once

#include "IState.hpp"
#include "stateManager.hpp"
#include <iostream>

class splashState : public IState {
private:
	bool m_closeApp = false;
public:
	sf::RectangleShape rectBox;
	void init() override {
		std::cout << "Hello from splash state \n";
		rectBox.setSize({ 100.f,600.f });
		rectBox.setFillColor(sf::Color::Magenta);
		rectBox.setPosition({ 300.f, 300.f });
	};
	void handleEvent(const sf::Event& event) override {
		if (const auto* key = event.getIf<sf::Event::KeyReleased>()) {
			if (key->code == sf::Keyboard::Key::Q) m_closeApp = true;
		}
	};
	void update(float dt) override {};
	void render(sf::RenderWindow& window) override {
		window.draw(rectBox);
	};
	void onEnter() override {}
	void onExit() override {}

	// State policies
	bool updateBelow() override { return 0; };
	bool renderBelow() override { return 0; };
	bool shouldCloseApp() const override
	{
		return m_closeApp;
	}
};