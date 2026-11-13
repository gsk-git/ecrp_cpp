#pragma once

#include "IState.hpp"
#include <SFML/Graphics/Sprite.hpp>

class splash : public IState {
public:
	// Member variables
	sf::RectangleShape rectBox;
	sf::Texture m_texture;
	std::optional<sf::Sprite> logosprite;
	std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
	
	// Member functions
	splash(sf::RenderWindow& windowHandle) : IState(windowHandle), m_texture(), logosprite(m_texture) {};
	
	void init() override {
		m_replaceGame = false;
		// Texture for the logo
		if (!m_texture.loadFromFile("res/img/splash.png"))
		{
			std::cerr << "Failed to load splash.png\n";
			return;
		}

		// Construct sprite AFTER texture loading
		logosprite.emplace(m_texture);

		// Now configure the sprite
		logosprite->setScale({
			800.f / static_cast<float>(m_texture.getSize().x),
			447.f / static_cast<float>(m_texture.getSize().y)
			});

		auto bounds = logosprite->getLocalBounds();

		logosprite->setOrigin({
			bounds.size.x * 0.5f,
			bounds.size.y * 0.5f
			});

		auto size = m_windowHandle.getSize();

		logosprite->setPosition({
			static_cast<float>(size.x) * 0.5f,
			static_cast<float>(size.y) * 0.5f
			});
		start;
	};
	void handleEvent(const sf::Event& event) override {
		if (const auto* key = event.getIf<sf::Event::KeyReleased>()) {
			if (key->code == sf::Keyboard::Key::Q) m_closeApp = true;
		}
	};
	void update(float dt) override {
		std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >= 5) m_replaceGame = true;
	};
	void render(sf::RenderWindow& window) override {
		window.draw(*logosprite);
	};
	void onEnter() override {}
	void onExit() override {}
	bool updateBelow() override { return 0; };
	bool renderBelow() override { return 0; };
	bool shouldCloseApp() const override { return m_closeApp; }
	bool shouldReplaceGame() const override { return m_replaceGame; }
	stateType getStateType() const override { return stateType::Splash; }
};