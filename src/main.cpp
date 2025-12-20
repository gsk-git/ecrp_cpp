// Including all necessary libraries
#include "config.hpp"
#include <Json/json.hpp>
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
#include <random>
#include <tuple>
#include <fstream>

using json = nlohmann::json;

// Loading game data from file
static void loadgame(json& open) {	
	std::ifstream loadfile("res/data.json");
	if (loadfile) {
		loadfile >> open;
		esrovar::PLAYER_POSITION.first = open.at("X");
		esrovar::PLAYER_POSITION.second = open.at("Y");
	}
}

// Saving game data
static void savegame(json& gfile) {
	
	esroops::PlayerData data = { esrovar::PLAYER_POSITION.first, esrovar::PLAYER_POSITION.second };
	gfile["X"] = data.m_playerposX;
	gfile["Y"] = data.m_playerposY;
	std::ofstream file("res/data.json");
	file << gfile.dump(4);
}

// Handling player input
static void InputHandler(esroops::Player& player, esroops::WorldManager& world, float& dt) {
	
	// Resetting movement direction and boost
	esrovar::movedirx = 0.f;
	esrovar::movediry = 0.f;
	esrovar::boost = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ? 2.0f : 1.f;
	
	// Setting running state
	player.m_IsRunning = esrovar::boost == 2.0f ? true : false;
	esrovar::totalspeed = esrovar::speed * esrovar::boost + esrovar::jumpboost;
	
	// Handling directional input
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
	
	// Creating directional vector
	sf::Vector2f direction({ esrovar::movedirx, esrovar::movediry });
	//std::cout << world.getTileType({ esrovar::PLAYER_POSITION.first + (direction.x * 0.2), esrovar::PLAYER_POSITION.second + (direction.y * 0.2) }) << " - " << esrovar::PLAYER_POSITION.first + direction.x << " - " << esrovar::PLAYER_POSITION.second + direction.y << "\n";
	
	// Check if directions are not 0
	if (direction.x != 0.f || direction.y != 0.f) {
		// Getting hypotenuse length
		float length = std::sqrt((direction.x * direction.x) + (direction.y * direction.y));
		// Normalizing vector movement
		direction /= length;
	}
	
	// Updating player movement
	player.move(direction * esrovar::totalspeed * dt);
	player.m_playerXY.x = esrovar::PLAYER_POSITION.first += direction.x * esrovar::totalspeed * dt;
	player.m_playerXY.y = esrovar::PLAYER_POSITION.second += direction.y * esrovar::totalspeed * dt;
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
			if (keyReleased->scancode == sf::Keyboard::Scan::V) {
				if (!esrovar::DebugMode)
					esrovar::DebugMode = true;
				else
					esrovar::DebugMode = false;
			}
			if (keyReleased->scancode == sf::Keyboard::Scan::M) {
				if (!esrovar::Save)
					esrovar::Save = true;
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
		LOG("Splash Logo not found or path is incorrect");
	// Creating sprite for the logo
	sf::Sprite logoSprite(logoTexture);
	logoSprite.setScale(sf::Vector2f(512.f / static_cast<float>(logoTexture.getSize().x), 512.f / static_cast<float>(logoTexture.getSize().y)));
	// Set the origin to the center of the logo
	auto bounds = logoSprite.getLocalBounds();
	logoSprite.setOrigin(sf::Vector2f(bounds.size.x * 0.5f, bounds.size.y * 0.5f));
	logoSprite.setPosition(sf::Vector2f(static_cast<float>(splash.getSize().x) * 0.5f, static_cast<float>(splash.getSize().y) * 0.5f));
	// Chrono library to manage time
	auto start = std::chrono::steady_clock::now();
	// Set the splash window to be transparent
	while (splash.isOpen()) {
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

// Manages in-game clock
static void GameClock(sf::Clock& gameclock, sf::Time& elapsed, int& seconds, int& minutes, int& day) {
	elapsed += gameclock.restart();
	
	if (elapsed >= sf::seconds(1.0f)) {
		elapsed -= sf::seconds(1.0f);
		seconds++;
	}
	if (seconds >= 60) {
		seconds = 0;
		minutes++;
	}
	if (minutes >= 60) {
		minutes = 0;
		day++;
	}
}

// Generates a random world seed
[[nodiscard]] static inline uint32_t GenerateWorldSeed() noexcept {
	
	uint64_t t = static_cast<uint64_t>(
	std::chrono::high_resolution_clock::now().time_since_epoch().count());
	// xorshift mix to spread bits
	t ^= t >> 33; 
	t *= 0xff51afd7ed558ccdULL;
	t ^= t >> 33; 
	t *= 0xc4ceb9fe1a85ec53ULL;
	t ^= t >> 33;
	return static_cast<uint32_t>(t & 0xFFFFFFFFu);
}

// Starts the game
static void StartGame() {
	
	// Initializing game variables
	int fps = 0;
	int frames = 0;
	int seconds = 0;
	int minutes = 0;
	int day = 0;
	float dt = 0.0f;
	const uint32_t gameseed = 23091995;
	float playerX;
	float playerY;
	json gameJSON;
	std::string tileType;
	std::mt19937 rng(gameseed);
	std::uniform_int_distribution dist(0, 5);
	std::tuple<int, int> playerXY = { 0, 0 };
	std::tuple<int, int> currentChunk = { 0, 0 };
	std::tuple<int, int> previousChunk = { 0, 0 };
	std::tuple<int, int> swapChunk = { 0, 0 };
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow gamewin(desktop, "ESRO", sf::Style::None);
	esrovar::GameWindow = std::move(gamewin);
	
	// Loading Assets
	esrofn::LoadSpriteSheetsnew();
	esrofn::LoadFonts();
	
	// loading gamedata
	loadgame(gameJSON);
	
	// Initializing game objects
	sf::View view;
	sf::Clock clock;
	sf::Clock fpsclock;
	sf::Clock gameclock;
	sf::Time elapsed = sf::Time::Zero;
	esroops::Player player;
	esroops::PlayerData data{};
	esroops::WorldManager world(esrovar::PLAYER_POSITION, gameseed);
	esroops::HudBox hudbox({ 350, 300 }, { 10.f, 10.f }, sf::Color(0, 0, 0, 200), sf::Color::White, 2.f);
	esroops::HudText playertext(esrovar::mainfont, { hudbox.getPosition().x + 10.f, 22.5f * 1 }, sf::Color::White, sf::Text::Regular, 18);
	esroops::HudText playerpos(esrovar::mainfont, { hudbox.getPosition().x + 10.f, 22.5f * 2 }, sf::Color::White, sf::Text::Regular, 18);
	esroops::HudText playerxy(esrovar::mainfont, { hudbox.getPosition().x + 10.f, 22.5f * 3 }, sf::Color::White, sf::Text::Regular, 18);
	esroops::HudText cchunkxy(esrovar::mainfont, { hudbox.getPosition().x + 10.f, 22.5f * 4 }, sf::Color::White, sf::Text::Regular, 18);
	esroops::HudText pchunkxy(esrovar::mainfont, { hudbox.getPosition().x + 10.f, 22.5f * 5 }, sf::Color::White, sf::Text::Regular, 18);
	esroops::HudText activechunks(esrovar::mainfont, { hudbox.getPosition().x + 10.f, 22.5f * 6 }, sf::Color::White, sf::Text::Regular, 18);
	esroops::HudText fpsrate(esrovar::mainfont, { hudbox.getPosition().x + 10.f, 22.5f * 7 }, sf::Color::White, sf::Text::Regular, 18);
	esroops::HudText gametime(esrovar::mainfont, { hudbox.getPosition().x + 10.f, 22.5f * 8 }, sf::Color::White, sf::Text::Regular, 18);
	esroops::HudText tiletype(esrovar::mainfont, { hudbox.getPosition().x + 10.f, 22.5f * 9 }, sf::Color::White, sf::Text::Regular, 18);
	
	// Setting world seed and player position
	world.m_world_seed = dist(rng);
	player.setPosition({esrovar::PLAYER_POSITION.first, esrovar::PLAYER_POSITION.second});
	
	// Setting camera center 
	sf::Vector2f cameraCenter = player.getPosition();
	
	// Adding updatable systems
	std::vector <esroops::IUpdatable*> systemdelta;
	systemdelta.push_back(&player);
	
	//Init View
	sf::Vector2f viewArea = {esrovar::SCRWDT, esrovar::SCRHGT};
	view.setSize(viewArea);
	view.setCenter(cameraCenter);	
	
	// Setting framerate limit
	esrovar::GameWindow.setVerticalSyncEnabled(true);
	
	//Main game loop
	while (esrovar::GameWindow.isOpen()) {
		
		// Delta time management
		dt = clock.restart().asSeconds();
		GameClock(gameclock, elapsed, seconds, minutes, day);
		
		// Increment Frames
		frames++;
		
		// Maintaining game wide delta time
		for (auto system : systemdelta) {
			system->update(dt);
		}
		
		//Handling windows events
		ProcessWindowEvents(player);
		
		// Handling player inputs (keyboard)
		InputHandler(player, world, dt);
		
		// Setting strings to HUD elements
		if (fpsclock.getElapsedTime().asSeconds() >= 1.0f) {
			fps = frames / static_cast<int>(fpsclock.getElapsedTime().asSeconds());
			frames = 0;
			fpsclock.restart();
		}
		
		playerX = esrovar::PLAYER_POSITION.first;
		playerY = esrovar::PLAYER_POSITION.second;
		swapChunk = esrofn::getChunkXY(esrovar::PLAYER_POSITION);
		playerXY = esrofn::getPlayerXY(esrovar::PLAYER_POSITION);
		tileType = world.getTileType(esrovar::PLAYER_POSITION);
		world.m_playerchunk_X = static_cast<float>(std::get<0>(swapChunk));
		world.m_playerchunk_Y = static_cast<float>(std::get<1>(swapChunk));
		
		// Updating current and previous chunk values
		if (swapChunk != currentChunk) {
			previousChunk = currentChunk;
			currentChunk = swapChunk;
			world.getRequiredChunks();
		}
		
		// New required chunks will be generated for every 5th frame
		if (frames % world.m_chunkGenerationLimit == 0) world.update(dt);
		
		// Updating player, world and HUD elements
		player.update(dt);
		
		// Smooth logical camera
		sf::Vector2f targetCenter = player.getPosition();
		cameraCenter += (targetCenter - cameraCenter) * (2.1f * dt);
		sf::Vector2f drawCenter = cameraCenter;
		drawCenter.x = std::floor(drawCenter.x);
		drawCenter.y = std::floor(drawCenter.y);
		view.setCenter(drawCenter);
		
		// Saving game data
		if (esrovar::Save) {
			savegame(gameJSON);
			esrovar::Save = false;
		}		
		
		// Initializating and generating world chunks, player, view and UI elements.
		esrovar::GameWindow.clear();
		esrovar::GameWindow.setView(view);
		world.f_drawChunks(esrovar::GameWindow);
		if (esrovar::ChunkBorder) world.ChunkBorders(esrovar::GameWindow);
		esrovar::GameWindow.draw(player);
		esrovar::GameWindow.setView(esrovar::GameWindow.getDefaultView());
		if (esrovar::DebugMode) {
			// Updating HUD elements
			playertext.setString("Player State: " + std::string(esrovar::kStateNames[esrovar::to_index(player.m_StateEnum)]));
			playerpos.setString(std::format("Global POS: X::{:.2f}, Y::{:.2f}", playerX, playerY));
			playerxy.setString(std::format("Player POS: X::{}, Y::{}", std::get<0>(playerXY), std::get<1>(playerXY)));
			cchunkxy.setString(std::format("currentChunk: X::{}, Y::{}", (std::get<0>(currentChunk)), std::get<1>(currentChunk)));
			pchunkxy.setString(std::format("previousChunk: X::{}, Y::{}", std::get<0>(previousChunk), std::get<1>(previousChunk)));
			tiletype.setString(std::format("TileType: {}", tileType));
			activechunks.setString(std::format("activeChunk: {}", world.m_active_chunks.size()));
			fpsrate.setString(std::format("FPS: {}", fps));
			gametime.setString(std::format("Game Time : {:0>2}:{:0>2}", minutes, seconds));
			esrovar::GameWindow.draw(hudbox);
			esrovar::GameWindow.draw(playertext);
			esrovar::GameWindow.draw(playerpos);
			esrovar::GameWindow.draw(playerxy);
			esrovar::GameWindow.draw(cchunkxy);
			esrovar::GameWindow.draw(pchunkxy);
			esrovar::GameWindow.draw(tiletype);
			esrovar::GameWindow.draw(activechunks);
			esrovar::GameWindow.draw(fpsrate);
			esrovar::GameWindow.draw(gametime);
		}
		esrovar::GameWindow.display();
	}	
}

//  Game main function
int main() {
	
	// Displays splash image
	RunSplash();	
	// Starts game
	StartGame();	
	// Exits game
	return EXIT_SUCCESS;
}