// Including necessary libraries
#include "config.hpp"
#include <iostream>
#include <random>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <map>
#include <set>
#include <string>
#include <system_error>
#include <utility>

// Global variables
namespace esrovar {

	int pixel_size = 50u;
	int frame_count = 0u;
	int scale = 2u;
	int seed = 23091995;
	float player_size = 50.0f;
	float timer = 0.0f;
	float speed = 100.0f;
	int chunkrea = CHUNK_SIZE * pixel_size;
	float movedirx = 0.0f;
	float movediry = 0.0f;
	float boost = 0.0f;
	float dt = 0.0f;
	float totalspeed = 0.0f;
	char format[5] = ".png";
	std::string TileImagePATH = "res/tile.png";
	std::string PlayerSpriteImagePATH = "res/player_sprite/";
	std::string states[5] = { "idle","walk","slash","jump","sit" };
	std::string FaceDirection[4] = { "up","left","down","right"};
	sf::VideoMode desktop = desktop.getDesktopMode();
	sf::RenderWindow GameWindow(desktop, "ESRO", sf::Style::None);
	std::map <std::string, sf::Texture> TextureFace;
	std::map<std::string, std::string> TexturePath = {
		{states[0], PlayerSpriteImagePATH + states[0] + format},
		{states[1], PlayerSpriteImagePATH + states[1] + format},
		{states[3], PlayerSpriteImagePATH + states[3] + format},
		{states[4], PlayerSpriteImagePATH + states[4] + format},
		{states[2], PlayerSpriteImagePATH + states[2] + format}};
	std::map<std::string, sf::IntRect> TextureRects = {
	{"up",    sf::IntRect({0 * PLAYER_SPRITE, 0 * PLAYER_SPRITE}, {PLAYER_SPRITE, PLAYER_SPRITE})},
	{"left",  sf::IntRect({0 * PLAYER_SPRITE, 1 * PLAYER_SPRITE}, {PLAYER_SPRITE, PLAYER_SPRITE})},
	{"down",  sf::IntRect({0 * PLAYER_SPRITE, 2 * PLAYER_SPRITE}, {PLAYER_SPRITE, PLAYER_SPRITE})},
	{"right", sf::IntRect({0 * PLAYER_SPRITE, 3 * PLAYER_SPRITE}, {PLAYER_SPRITE, PLAYER_SPRITE})}};
	std::pair<int, int> PLAYER_POSITION = {0, 0};
} // namespace esrovar ends

// Global functions
namespace esrofn {

	void LoadSpriteSheets() {
		try {
			for (const auto& [name, path] : esrovar::TexturePath) {
				sf::Texture img;
				if (!img.loadFromFile(path))
					LOG("Image Path or File not found");
				esrovar::TextureFace.try_emplace(name, std::move(img));
			}
		}
		catch (const std::system_error& e) {
			std::cerr << "Error: " << e.what() << "\n";
		}        
	}

	int GenerateWorldSeed() {
		std::random_device randev;
		std::mt19937 gen(randev());
		std::uniform_int_distribution<int> dist(0, 2100000000);
		return dist(gen);
	}
} // namespace esrofn ends

// Global objects and classes
namespace esroops {

	Chunk::Chunk(int x, int y) {
		for (int y = 0; y < esrovar::CHUNK_SIZE; ++y)
			for (int x = 0; x < esrovar::CHUNK_SIZE; ++x) {
				tiles[x][y].type = BlockType::plains;
			}
		m_chunkX = x;
		m_chunkY = y;
		m_isGenerated = false;
	}

	Player::Player() {
		// Initializing member variables
		m_IsMoving = false;
		m_State = esrovar::states[0];
		m_direction = esrovar::FaceDirection[2];
		m_playerXY = sf::Vector2f(static_cast<float>(0.0f), static_cast<float>(0.0f));
		m_TotalFrames = 0;
		m_CurrentFrame = 0;
		m_AnimTimer = 0.0f;
		m_AnimDuration = 0.0f;
		m_health = 500u;
		// Default sprite and position
		m_playersprite.emplace(esrovar::TextureFace[m_State]);
		m_playersprite->setTextureRect(esrovar::TextureRects[m_direction]);
		m_playersprite->setPosition(m_playerXY);
	}

	WorldManager::WorldManager(std::pair<int, int> PLAYERXY) {    
		m_playerchunk_X = PLAYERXY.first;
		m_playerchunk_Y = PLAYERXY.second;
		m_active_chunks;
		m_world_seed = esrovar::seed;
		f_initialize_world();
	}

	Tile* Chunk::getTileData(int x, int y) {        
		return &tiles[x][y];
	}

	void Chunk::generate(const std::string& tilesheet, sf::Vector2i tilesize) {
	
		// Loading input tilesheet
		if (!m_tileset.loadFromFile(tilesheet))
			LOG("Tilesheet not found or path is incorrect");
		// Set vertex properties
		m_grid.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
		m_grid.resize(static_cast<size_t>(esrovar::CHUNK_SIZE) * esrovar::CHUNK_SIZE * 6);
		// Instantiating vertex grid
		for (int y = 0; y < esrovar::CHUNK_SIZE; ++y) {
			for (int x = 0; x < esrovar::CHUNK_SIZE; ++x) {
				
				// tileIndex identifies where the tile will sit in this grid
				int tileIndex = (x + y * esrovar::CHUNK_SIZE) * 6;
				float chunkOffsetX = static_cast<float>(m_chunkX * esrovar::CHUNK_SIZE * tilesize.x);
				float chunkOffsetY = static_cast<float>(m_chunkY * esrovar::CHUNK_SIZE * tilesize.y);
				// Tile sheet index for now default selection is plains
				int tu = 0;
				int tv = 0;
				// XY of independent tile
				auto tx = chunkOffsetX + static_cast<float>(x * tilesize.x);
				auto ty = chunkOffsetY + static_cast<float>(y * tilesize.y);
				// XY for independent tile texture
				auto texX = static_cast<float>(tu * tilesize.x);
				auto texY = static_cast<float>(tv * tilesize.y);
				// Triangle 1
				m_grid[ static_cast<size_t>(tileIndex) + 0].position    = sf::Vector2f(tx, ty);
				m_grid[ static_cast<size_t>(tileIndex) + 1].position    = sf::Vector2f(tx + tilesize.x, ty);
				m_grid[ static_cast<size_t>(tileIndex) + 2].position    = sf::Vector2f(tx, ty + tilesize.y);
				m_grid[ static_cast<size_t>(tileIndex) + 0].texCoords   = sf::Vector2f(texX, texY);
				m_grid[ static_cast<size_t>(tileIndex) + 1].texCoords   = sf::Vector2f(texX + tilesize.x, texY);
				m_grid[ static_cast<size_t>(tileIndex) + 2].texCoords   = sf::Vector2f(texX, texY + tilesize.y);
				// Triangle 2
				m_grid[ static_cast<size_t>(tileIndex) + 3].position    = sf::Vector2f(tx + tilesize.x, ty);
				m_grid[ static_cast<size_t>(tileIndex) + 4].position    = sf::Vector2f(tx + tilesize.x, ty + tilesize.y);
				m_grid[ static_cast<size_t>(tileIndex) + 5].position    = sf::Vector2f(tx, ty + tilesize.y);
				m_grid[ static_cast<size_t>(tileIndex) + 3].texCoords   = sf::Vector2f(texX + tilesize.x, texY);
				m_grid[ static_cast<size_t>(tileIndex) + 4].texCoords   = sf::Vector2f(texX + tilesize.x, texY + tilesize.y);
				m_grid[ static_cast<size_t>(tileIndex) + 5].texCoords   = sf::Vector2f(texX, texY + tilesize.y);
				// Setting cell's type as a plain
				tiles[x][y].type = BlockType::plains;
			}
		}
		// Updating chunk generated status
		m_isGenerated = true;
	}

	void Chunk::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform();
		states.texture = &m_tileset;
		target.draw(m_grid, states);
	}    

	void Player::update(float dt) {
		// Flag check for player movement
		m_IsMoving = (esrovar::movedirx != 0 || esrovar::movediry != 0);
		if (m_IsMoving)
			// Player is walking
			m_State = esrovar::states[1];
		else
			// Player is idle
			m_State = esrovar::states[0];
		// Update sprite and position
		m_playersprite->setTexture(esrovar::TextureFace[m_State]);
		m_playersprite->setTextureRect(esrovar::TextureRects[m_direction]);
		m_playersprite->setPosition(m_playerXY);
	}

	void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		// Draws player object
		states.transform *= getTransform();
		target.draw(*m_playersprite, states);
	}

	void WorldManager::f_initialize_world() {
		// Initializing set to hold required chunk in that frame
		std::set<std::pair<int, int>> RequiredChunks;
		// Calculating visible chunk radius around player
		for (int x = -esrovar::CHUNK_RADIUS; x <= esrovar::CHUNK_RADIUS; ++x) {
			for (int y = -esrovar::CHUNK_RADIUS; y <= esrovar::CHUNK_RADIUS; ++y) {
				int targetX = m_playerchunk_X + x;
				int targetY = m_playerchunk_Y + y;
				RequiredChunks.insert(std::make_pair(targetX, targetY));
			}
		}

		// Generating initial chunk within player's radius
		for (auto& [cx, cy] : RequiredChunks) {
			if (!m_active_chunks.contains({ cx, cy })) {
				//std::cout << cx << ", " << cy << "\n";
				Chunk _chunk(cx, cy);
				_chunk.generate(esrovar::TileImagePATH, sf::Vector2i({esrovar::pixel_size, esrovar::pixel_size }));
				m_active_chunks.insert({{cx, cy}, _chunk});
			}
		}
	}

	void WorldManager::f_drawChunks(sf::RenderWindow& window) {
		for (auto& [coords, _chunk] : m_active_chunks) {
			window.draw(_chunk);
		}
	}
} // namespace esroops ends