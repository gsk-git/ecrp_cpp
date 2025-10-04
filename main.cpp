// Included necessary libraries
#include "config.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/View.hpp>
#include <chrono>
#include <Windows.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <optional>
#include <string>

// Handles input
static void InputHandler(esroops::Player& player) {
	esrovar::movedirx = 0.f;
	esrovar::movediry = 0.f;
	esrovar::boost = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ? 2.0f : 1.0f;
	esrovar::totalspeed = esrovar::speed * esrovar::boost;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
		esrovar::movediry = -esrovar::totalspeed;
		player.m_DirectionEnum = esrovar::Directions::up;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
		esrovar::movedirx = -esrovar::totalspeed;
		player.m_DirectionEnum = esrovar::Directions::left;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
		esrovar::movediry = esrovar::totalspeed;
		player.m_DirectionEnum = esrovar::Directions::down;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
		esrovar::movedirx = esrovar::totalspeed;
		player.m_DirectionEnum = esrovar::Directions::right;
	}
}

// Processes window events
static void ProcessWindowEvents() {
	// Process window events
	while (const std::optional event = esrovar::GameWindow.pollEvent()) {
		if (event -> is <sf::Event::Closed>())
			esrovar::GameWindow.close();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
			esrovar::GameWindow.close();
		}
	}
}

// Make SFML window transparent
static void SplashWindow(sf::RenderWindow& window) {
	HWND hwnd = static_cast<HWND>(window.getNativeHandle());
	LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	// Set alpha to 0 for full transparency and click-through
	SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

// Runs the splash screen
static void RunSplash() {
	// Create a transparent splash window
	sf::RenderWindow splash(sf::VideoMode({ 512, 512 }), "Splash", sf::Style::None);
	// HWND handle for the splash window
	SplashWindow(splash);
	// Texture for the logo
	sf::Texture logoTexture;
	if (!logoTexture.loadFromFile("res/splash.png"))
		LOG("Logo not found or path is incorrect");
	// Creating sprite for the logo
	sf::Sprite logoSprite(logoTexture);
	logoSprite.setScale(sf::Vector2f(static_cast<float>(512.f) / logoTexture.getSize().x, static_cast<float>(512.f) / logoTexture.getSize().y));
	// Set the origin to the center of the logo
	auto bounds = logoSprite.getLocalBounds();
	logoSprite.setOrigin(sf::Vector2f(bounds.size.x * 0.5f, bounds.size.y * 0.5f));
	logoSprite.setPosition(sf::Vector2f(splash.getSize().x * 0.5f, splash.getSize().y * 0.5f));
	// Chrono library to manage time
	auto start = std::chrono::steady_clock::now();
	// Set the splash window to be transparent
	while (splash.isOpen()) {
		while (const std::optional event = splash.pollEvent()) {
			if (event->is <sf::Event::Closed>())
				splash.close();
		}
		// Check if 10 seconds passed
		auto now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >= 2) {
			splash.close();
		}
		splash.clear(sf::Color(0,0,0,0));
		splash.draw(logoSprite);
		splash.display();
	}
}

// Starts the game
static void StartGame() {
	
	// Load Assets
	//esrofn::LoadSpriteSheets();
	esrofn::LoadSpriteSheetsnew();
	
	// Initializing GameWindow and framerate
	sf::Clock clock;
	esroops::WorldManager world(esrovar::PLAYER_POSITION);
	esroops::Player player;
	std::vector <esroops::IUpdatable*> systemdelta;
	systemdelta.push_back(&player);
	
	//Init View
	sf::View view;
	sf::Vector2f viewArea = { esrovar::SCRWDT, esrovar::SCRHGT };
	view.setSize(viewArea);
	
	//Main game loop
	while (esrovar::GameWindow.isOpen()) {
		
		// Delta time management
		esrovar::dt = clock.restart().asSeconds();
		esrovar::GameWindow.setFramerateLimit(esrovar::FPS);
		
		//CloseGameWindow on close
		ProcessWindowEvents();
		
		// Spike for maintain system wide delta time management
		for (auto system : systemdelta) {
			system->update(esrovar::dt);
		}
		
		// Handle inputs
		InputHandler(player);
		
		// Get directions vector
		sf::Vector2f direction({ esrovar::movedirx, esrovar::movediry });
		
		// Check if directions are not 0
		if (direction.x != 0.f || direction.y != 0.f) {
			// Getting hypotenuse length
			float length = std::sqrt((direction.x * direction.x) + (direction.y * direction.y));
			// Normalize
			direction /= length;
		}
		
		// Move and update player
		player.move(direction * esrovar::totalspeed * esrovar::dt);
		player.update(esrovar::dt);
		
		// Smooth scroll view
		sf::Vector2f viewcenter = view.getCenter();
		sf::Vector2f targetcenter = player.getPosition();
		float lerpfactor = 5.0f * esrovar::dt;
		view.setCenter(viewcenter + (targetcenter - viewcenter) * lerpfactor);
		
		// GameWindow initialization
		esrovar::GameWindow.clear();
		esrovar::GameWindow.setView(view);
		world.f_drawChunks(esrovar::GameWindow);
		esrovar::GameWindow.draw(player);   
		esrovar::GameWindow.setView(esrovar::GameWindow.getDefaultView());
		esrovar::GameWindow.display();
	}
}	

//  Game Main Function
int main() {
	
	// Displays Default Splash Image
	RunSplash();
	
	//Starts Game
	StartGame();
	return EXIT_SUCCESS;
}