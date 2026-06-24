// Including all necessary libraries
#include <cmath>
#include <tuple>
#include <vector>
#include <chrono>
#include <string>
#include <random>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include "headers/config.hpp"
#include "headers/playerManager.hpp"
#include "headers/chunkManager.hpp"
#include "headers/worldManager.hpp"
#include <Windows.h>
#include <dwmapi.h>
#include <Json/json.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <utility>
#include <cstdint>

const uint32_t magicNumber = 2135284562;

// Loading game data from file
static inline void loadgame() {

	nlohmann::json savefile;
	std::ifstream loadfile(playerFileURI + saveFile);
	uint32_t mNumber = 0;
	if (loadfile.is_open()) {
		loadfile >> savefile;
			PLAYER_POSITION.first = savefile.at("X");
			PLAYER_POSITION.second = savefile.at("Y");
			loadfile.close();
	}
}

// Saving game data
static inline void savegame() {
	
	nlohmann::json savefile;
	// User file directory
	std::filesystem::create_directories(playerFileURI);	
	// Creat full path for data.json
	std::string fullPATH = playerFileURI + saveFile;	
	// Creating save info as json
	PlayerData data = { 
		PLAYER_POSITION.first, 
		PLAYER_POSITION.second 
	};
	// Adding player position to json object
	savefile["magicNumber"] = magicNumber;
	savefile["version"] = "0.1.0";
	savefile["X"] = data.m_playerposX;
	savefile["Y"] = data.m_playerposY;
	// Opening data.json
	std::ofstream file(fullPATH);
	if (file.is_open()) {
		// Writing player data onto file
		file << savefile.dump();
		// Closing file
		file.close();
	}
	else { 
		std::cout << "Save File corrupted, check if the below path exists \n" << fullPATH; 
	}
}

// Handling player input
static inline void InputHandler(Player& player, float dt) {

	// Resetting movement direction and boost
	movedirx = 0.f;
	movediry = 0.f;
	boost = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ? 2.0f : 0.6f;

	// Setting running state
	player.m_IsRunning = boost == 2.0f ? true : false;
	totalspeed = speed * boost + jumpboost;

	// Handling directional input
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
		movediry = -totalspeed;
		player.m_DirectionEnum = Directions::up;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
		movediry = totalspeed;
		player.m_DirectionEnum = Directions::down;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
		movedirx = -totalspeed;
		player.m_DirectionEnum = Directions::left;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
		movedirx = totalspeed;
		player.m_DirectionEnum = Directions::right;
	}

	// Creating directional vector
	sf::Vector2f direction({ movedirx, movediry });

	// Check if directions are not 0
	if (direction.x != 0.f || direction.y != 0.f) {
		// Getting hypotenuse length
		float length = std::sqrt((direction.x * direction.x) + (direction.y * direction.y));
		// Normalizing vector movement
		direction /= length;
	}

	// Updating player movement
	player.move(direction * totalspeed * dt);
	player.m_playerXY.x = PLAYER_POSITION.first += direction.x * totalspeed * dt;
	player.m_playerXY.y = PLAYER_POSITION.second += direction.y * totalspeed * dt;
}

// Processing window events
static inline void ProcessWindowEvents(Player& player, sf::RenderWindow& gamewin) {
	// Process window events
	while (const std::optional event = gamewin.pollEvent()) {
		if (event->is <sf::Event::Closed>())
			gamewin.close();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
			gamewin.close();
		}
		if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
			if (keyReleased->scancode == sf::Keyboard::Scan::LControl) {
				if (!player.m_IsSitting)
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

// Generates the suik bmnhplash screen
static void RunSplash() {

	// Create a borderless window
	sf::RenderWindow splash(sf::VideoMode({ 800, 600 }), "Floating Character", sf::Style::None);
	HWND hwnd = splash.getNativeHandle();
	// Set window styles for transparency and click-through
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	// Make background fully transparent using DWM
	MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea(hwnd, &margins);
	// Set window as 'Always on Top'
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
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
		if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >= 20) {
			splash.close();
		}
		splash.clear(sf::Color(0,0,0,0));
		splash.draw(logoSprite);
		splash.display();
	}
}

// Manages in-game clock
static inline void GameClock(sf::Clock& gameclock, sf::Time& elapsed, int& seconds, int& minutes, int& day) {
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
	float playerX;
	float playerY;
	nlohmann::json gameJSON;
	const uint32_t gameseed = 23091995;
	std::string tileType;
	std::mt19937 rng(gameseed);
	std::uniform_int_distribution dist(0, 5);
	std::tuple<int, int> playerXY = { 0, 0 };
	std::tuple<int, int> currentChunk = { 0, 0 };
	std::tuple<int, int> previousChunk = { 0, 0 };
	std::tuple<int, int> swapChunk = { 0, 0 };
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow gamewin(desktop, "ESRO™ v0.1.0", sf::Style::None);
	
	// Loading Assets
	LoadSpriteSheetsnew();
	esrofn::LoadFonts();
	
	// loading gamedata
	// loadgame();
	
	// Initializing game objects
	sf::View view;
	sf::Clock clock;
	sf::Clock fpsclock;
	sf::Clock debugUIclock;
	sf::Clock gameclock;
	float worlUpdateClock = 0.0f;
	sf::Time elapsed = sf::Time::Zero;
	Player player;
	esroops::WorldManager world(PLAYER_POSITION, gameseed);
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
	player.setPosition({ PLAYER_POSITION.first, PLAYER_POSITION.second });
	
	// Setting camera center 
	sf::Vector2f cameraCenter = player.getPosition();	
	
	//Init View
	sf::Vector2f viewArea = {esrovar::SCRWDT, esrovar::SCRHGT};
	view.setSize(viewArea);
	view.setCenter(cameraCenter);	
	
	// Setting VSync and mouse cursor
	gamewin.setVerticalSyncEnabled(false);
	gamewin.setMouseCursorVisible(false);
	
	//Main game loop
	while (gamewin.isOpen()) {
		
		// Delta time management
		dt = clock.restart().asSeconds();
		GameClock(gameclock, elapsed, seconds, minutes, day);
		worlUpdateClock += dt;		
		
		// Increment Frames
		frames++;
		
		//Handling windows events
		ProcessWindowEvents(player, gamewin);
		
		// Handling player inputs (keyboard)
		InputHandler(player, dt);
		
		// Setting strings to HUD elements
		if (fpsclock.getElapsedTime().asSeconds() >= 1.0f) {
			fps = frames / static_cast<int>(fpsclock.getElapsedTime().asSeconds());
			frames = 0;
			fpsclock.restart();
		}
		
		// Updating player position, chunk and tile type
		playerX = PLAYER_POSITION.first;
		playerY = PLAYER_POSITION.second;
		
		// Getting current chunk and player XY position
		swapChunk = esrofn::getChunkXY(PLAYER_POSITION);
		
		// Getting player XY position and tile type
		playerXY = esrofn::getPlayerXY(PLAYER_POSITION);
		tileType = world.getTileType(PLAYER_POSITION);
		
		// Updating world player chunk position
		world.m_playerchunk_X = static_cast<float>(std::get<0>(swapChunk));
		world.m_playerchunk_Y = static_cast<float>(std::get<1>(swapChunk));
		
		// Updating current and previous chunk values
		if (swapChunk != currentChunk) {
			previousChunk = currentChunk;
			currentChunk = swapChunk;
			world.getRequiredChunks();
		}
		
		// New required chunks will be generated for every 5ms to prevent frame drops, and world will be updated with new chunks
		if (worlUpdateClock >= 0.05f) {
			world.update(dt);
			worlUpdateClock = 0.0f;
		}
		
		player.update(dt);
		
		// Smooth logical camera
		sf::Vector2f targetCenter = player.getPosition();
		cameraCenter += (targetCenter - cameraCenter) * (1.5f * dt);
		sf::Vector2f drawCenter = cameraCenter;
		drawCenter.x = std::floor(drawCenter.x);
		drawCenter.y = std::floor(drawCenter.y);
		view.setCenter(drawCenter);
		
		// Saving game data
		if (esrovar::Save) {
			savegame();
			esrovar::Save = false;
		}		
		
		// Initializating and generating world chunks, player, view and UI elements.
		gamewin.clear();
		gamewin.setView(view);
		world.f_drawChunks(gamewin);
		if (esrovar::ChunkBorder) world.ChunkBorders(gamewin);
		gamewin.draw(player);
		gamewin.setView(gamewin.getDefaultView());
		if (esrovar::DebugMode) {
			// Updating debug UI elements every 0.25 seconds
			if(debugUIclock.getElapsedTime().asSeconds() >= 0.25f)
			{
				playertext.setString("Player State: " + std::string(kStateNames[to_index(player.m_StateEnum)]));
				playerpos.setString(std::format("Global POS: X::{:.2f}, Y::{:.2f}", playerX, playerY));
				playerxy.setString(std::format("Player POS: X::{}, Y::{}", std::get<0>(playerXY), std::get<1>(playerXY)));
				cchunkxy.setString(std::format("currentChunk: X::{}, Y::{}", (std::get<0>(currentChunk)), std::get<1>(currentChunk)));
				pchunkxy.setString(std::format("previousChunk: X::{}, Y::{}", std::get<0>(previousChunk), std::get<1>(previousChunk)));
				tiletype.setString(std::format("TileType: {}", tileType));
				activechunks.setString(std::format("activeChunk: {}", world.m_active_chunks.size()));
				fpsrate.setString(std::format("FPS: {}", fps));
				gametime.setString(std::format("Game Time : {:0>2}:{:0>2}", minutes, seconds));
				debugUIclock.restart();
			}
			// Drawing debug UI elements
			gamewin.draw(hudbox);
			gamewin.draw(playertext);
			gamewin.draw(playerpos);
			gamewin.draw(playerxy);
			gamewin.draw(cchunkxy);
			gamewin.draw(pchunkxy);
			gamewin.draw(tiletype);
			gamewin.draw(activechunks);
			gamewin.draw(fpsrate);
			gamewin.draw(gametime);
		}
		gamewin.display();
	}	
}

//  Game main function
int main(int argCount, char* argVector[]) {

	bool SPLASH_FLAG = false;
	
	// Handling command line arguments
	if (argCount > 1) {
		
		for (int i = 0; i < argCount; i++) {
			// Checking for debug mode and no-splash flags in command line arguments
			if (std::string(argVector[i]) == "--debug" || std::string(argVector[i]) == "-d") {
				esrovar::DebugMode = true;
			}
			// Checking for no-splash flag in command line arguments
			else if (std::string(argVector[i]) == "--no-splash" || std::string(argVector[i]) == "-ns") {
				SPLASH_FLAG = true;
			}
		}
	}
	
	// Displays splash image
	if (!SPLASH_FLAG) RunSplash();
	
	// Starts game
	StartGame();
	
	// Exits game
	return EXIT_SUCCESS;
}