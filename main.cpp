// Including all necessary libraries
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

// Handling player input
static void InputHandler(esroops::Player& player) {
	esrovar::movedirx = 0.f;
	esrovar::movediry = 0.f;
	esrovar::boost = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ? 2.0f : 0.5f;
	player.m_IsRunning = esrovar::boost == 2.0f ? true : false;
	esrovar::totalspeed = esrovar::speed * esrovar::boost + esrovar::jumpboost;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
		esrovar::movediry = -esrovar::totalspeed;
		player.m_DirectionEnum = esrovar::Directions::up;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
		esrovar::movediry = esrovar::totalspeed;
		player.m_DirectionEnum = esrovar::Directions::down;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
		esrovar::movedirx = -esrovar::totalspeed;
		player.m_DirectionEnum = esrovar::Directions::left;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
		esrovar::movedirx = esrovar::totalspeed;
		player.m_DirectionEnum = esrovar::Directions::right;
	}
}

// Processing window events
static void ProcessWindowEvents(esroops::Player& player) {
	// Process window events
	while (const std::optional event = esrovar::GameWindow.pollEvent()) {
		if (event -> is <sf::Event::Closed>())
			esrovar::GameWindow.close();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
			esrovar::GameWindow.close();
		}
		if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
			if (keyReleased->scancode == sf::Keyboard::Scan::LControl) {
				if(!player.m_IsSitting)
					player.m_IsSitting = true;
				else
					player.m_IsSitting = false;
			}
			if (keyReleased->scancode == sf::Keyboard::Scan::Space) {
				player.m_IsJumping = true;
			}
			if (keyReleased->scancode == sf::Keyboard::Scan::B) {
				if (!esrovar::ChunkBorder)
					esrovar::ChunkBorder = true;
				else
					esrovar::ChunkBorder = false;
			}
			if (keyReleased->scancode == sf::Keyboard::Scan::C) {
				if (!esrovar::ChunkColor)
					esrovar::ChunkColor = true;
				else
					esrovar::ChunkColor = false;
			}
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

// Generates the splash screen
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
	float dt = 0.0f;
	
	// Loading Assets
	esrofn::LoadSpriteSheetsnew();
	esrofn::LoadTileSheet();
	esrofn::LoadFonts();
	
	// Initializing game objects
	sf::Clock clock;
	esroops::Player player;
	esrovar::worldseed = esrofn::GenerateWorldSeed();
	esroops::WorldManager world(esrovar::PLAYER_POSITION, esrovar::worldseed);
	std::vector <esroops::IUpdatable*> systemdelta;
	systemdelta.push_back(&player);
	
	// UI Elements
	sf::Text text(esrovar::mainfont);
	sf::RectangleShape box;
	text.setCharacterSize(18);
	text.setFillColor(sf::Color::Green);
	text.setStyle(sf::Text::Bold);
	text.setString("Player State: None ");
	auto lb = text.getLocalBounds();
	box.setSize(sf::Vector2f({200.f, lb.size.y + 20.f}));
	box.setFillColor(sf::Color(0, 0, 0, 200));
	box.setPosition(sf::Vector2f({ 10.f, 10.f }));
	box.setOutlineColor(sf::Color::White);
	box.setOutlineThickness(2.f);
	text.setPosition(sf::Vector2f({ box.getPosition().x + 10.f, box.getPosition().y + 10.f}));	
	//Init View
	sf::View view;
	sf::Vector2f viewArea = { esrovar::SCRWDT, esrovar::SCRHGT };
	view.setSize(viewArea);
	//Main game loop
	while (esrovar::GameWindow.isOpen()) {
		
		// Delta time management
		dt = clock.restart().asSeconds();
		
		// Capping framerate @60fps
		esrovar::GameWindow.setFramerateLimit(esrovar::FPS);
		
		// Maintaining game wide delta time
		for (auto system : systemdelta) {
			system->update(dt);
		}
		
		//Handling windows events
		ProcessWindowEvents(player);
		
		// Handling player inputs (keyboard)
		InputHandler(player);
		
		// Ascertaining directions vector
		sf::Vector2f direction({ esrovar::movedirx, esrovar::movediry });
		
		// Check if directions are not 0
		if (direction.x != 0.f || direction.y != 0.f) {
			// Getting hypotenuse length
			float length = std::sqrt((direction.x * direction.x) + (direction.y * direction.y));
			// Normalizing vector movement
			direction /= length;
		}
		
		// Updating player movement
		player.move(direction * esrovar::totalspeed * dt);
		player.update(dt);
		world.update(esrovar::worldseed);
		text.setString("Player State: " + std::string(esrovar::kStateNames[esrovar::to_index(player.m_StateEnum)]));
		
		// Smoothening scroll view
		sf::Vector2f viewcenter = view.getCenter();
		sf::Vector2f targetcenter = player.getPosition();
		// Linear interpolation factor
		float lerpfactor = 5.0f * dt;
		view.setCenter(viewcenter + (targetcenter - viewcenter) * lerpfactor);
		
		// Initializating and generating world chunks, player, view and UI elements.
		esrovar::GameWindow.clear();
		esrovar::GameWindow.setView(view);
		world.f_drawChunks(esrovar::GameWindow);
		if (esrovar::ChunkBorder)
			world.ChunkBorders(esrovar::GameWindow);
		esrovar::GameWindow.draw(player);
		esrovar::GameWindow.setView(esrovar::GameWindow.getDefaultView());
		esrovar::GameWindow.draw(box);
		esrovar::GameWindow.draw(text);
		esrovar::GameWindow.display();
	}
}	

//  Game main function
int main() {
	
	// Displays splash image
	RunSplash();
	
	//Starts game
	StartGame();
	
	// Exits game
	return EXIT_SUCCESS;
}