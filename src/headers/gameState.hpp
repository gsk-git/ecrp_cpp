#pragma once

#include "IState.hpp"
#include <optional>
#include "config.hpp"
#include "playerManager.hpp"
#include "chunkManager.hpp"
#include "worldManager.hpp"
#include <iostream>

class gameState : public IState {
public:
	// Member variables
	sf::RectangleShape rectBox;
	gameState(sf::RenderWindow& windowHandle) : IState(windowHandle) {};
	
	// Member functions
	void init() override {
		std::cout << "Hello from splash state \n";
		rectBox.setSize({ 100.f,600.f });
		rectBox.setFillColor(sf::Color::Magenta);
		rectBox.setPosition(sf::Vector2f(static_cast<float>(m_windowHandle.getSize().x) * 0.5f, static_cast<float>(m_windowHandle.getSize().y) * 0.5f));
	};
	void handleEvent(const sf::Event& event) override {
		if (const auto* key = event.getIf<sf::Event::KeyReleased>()) {
			if (key->code == sf::Keyboard::Key::Q) m_closeApp = true;
			if (key->code == sf::Keyboard::Key::Escape) m_openPause = true;
		}
	};
	void update(float dt) override {};
	void render(sf::RenderWindow& window) override {
		window.draw(rectBox);
	};
	void onEnter() override {
		m_openPause = false;
	}
	void onExit() override {}
	bool updateBelow() override { return 0; };
	bool renderBelow() override { return 0; };
	bool shouldCloseApp() const override { return m_closeApp; }
	bool shouldOpenPause() const override { return m_openPause; }
	stateType getStateType() const override
	{
		return stateType::GameMenu;
	}
};