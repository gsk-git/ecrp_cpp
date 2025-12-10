// Including necessary libraries
#include "config.hpp"
#include <cmath>
#include <cstdint>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <map>
#include <string>
#include <utility>
#include <Windows.h>
#include <FastNoise/FastNoiseLite.h>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <format>
#include <tuple>
#include <vector>

// Global variables
namespace esrovar {

	int pixel_size = 50u;
	int frame_count = 0u;
	int scale = 2u;
	int seed = 23091995;
	float player_size = 50.0f;
	float timer = 0.0f;
	float speed = 100.0f;
	int chunk_area = CHUNK_SIZE * pixel_size;
	float movedirx = 0.0f;
	float movediry = 0.0f;
	float boost = 0.0f;
	float dt = 0.0f;
	float totalspeed = 0.0f;
	int jumpboost = 0;
	sf::VideoMode desktop = desktop.getDesktopMode();
	sf::RenderWindow GameWindow(desktop, "ESRO", sf::Style::None);
	FastNoiseLite noise;
	std::pair<float, float> PLAYER_POSITION = {0.0f, 0.0f};
	std::array<sf::Texture, StateCount> kTextures;
	sf::Font mainfont;
	sf::Texture tileset;
	std::vector<sf::Color> biome_colors = {
		sf::Color(170, 200, 120),   // 0 - Plains
		sf::Color(240, 224, 170),   // 1 - dirt
		sf::Color(216, 184, 100),   // 2 - Beach
		sf::Color(33, 87, 141),     // 3 - Ocean
		sf::Color(26, 123, 59),     // 4 - Jungle
		sf::Color(110, 118, 130),   // 5 - Mountains
	};
	std::vector<sf::Color> biome_variants = {
		// --- OCEAN & WATER ---
		sf::Color(23, 66, 145),     // 0 - Deep Ocean
		sf::Color(28, 107, 160),    // 1 - Ocean
		sf::Color(46, 139, 187),    // 2 - Shallow Ocean
		sf::Color(70, 160, 200),    // 3 - Lagoon / Coral Sea
		sf::Color(90, 190, 220),    // 4 - River / Freshwater
			
		// --- COAST ---
		sf::Color(240, 224, 170),   // 5 - Beach (warm sand)
		sf::Color(228, 193, 119),   // 6 - Desert Coast (dry sand)
		sf::Color(210, 190, 140),   // 7 - Rocky Shore
			
		// --- TEMPERATE ---
		sf::Color(170, 200, 120),   // 8 - Plains
		sf::Color(150, 185, 100),   // 9 - Meadow
		sf::Color(115, 150, 75),    // 10 - Forest
		sf::Color(34, 139, 69),     // 11 - Jungle
		sf::Color(98, 140, 88),     // 12 - Swamp / Marsh
			
		// --- ARID ---
		sf::Color(216, 184, 100),   // 13 - Desert
		sf::Color(190, 170, 90),    // 14 - Savannah
		sf::Color(150, 120, 65),    // 15 - Badlands
		sf::Color(180, 130, 70),    // 16 - Mesa / Clay Canyon
			
		// --- COLD ---
		sf::Color(110, 130, 150),   // 17 - Tundra
		sf::Color(90, 110, 140),    // 18 - Taiga (cold conifer forest)
			
		// --- SNOW & ICE ---
		sf::Color(230, 240, 255),   // 19 - Snow Field
		sf::Color(200, 220, 240),   // 20 - Ice Sheet
		sf::Color(180, 200, 225),   // 21 - Frozen Ocean
			
		// --- MOUNTAINS ---
		sf::Color(120, 130, 140),   // 22 - Mountain Base (rock)
		sf::Color(150, 160, 170),   // 23 - High Mountains (light rock)
		sf::Color(200, 210, 220),   // 24 - Snowy Peaks
	};
	bool ChunkBorder = false;
	bool DebugMode = false;
	bool Save = false;
	float globaldelta = 0.0f;
} // namespace esrovar ends

// Global functions
namespace esrofn {

	bool LoadSpriteSheetsnew() {
		// Loading all player textures
		for (std::size_t i = 0; i < esrovar::StateCount; ++i) {
			// Convert string_view → std::string for SFML
			std::string path{ esrovar::kTexturePaths[i] };
			if (!esrovar::kTextures[i].loadFromFile(path)) {
				esrovar::kTextures[i].setSmooth(false);
				LOG("Failed to load texture: " + path);
				return false;
			}
            // set smoothing (or disable) on success if desired
            esrovar::kTextures[i].setSmooth(false);
		}
		return true;
	}

	bool LoadTileSheet() {
		// Loading tile sheet
		if (!esrovar::tileset.loadFromFile("res/tile.png")) {
			LOG("Tilesheet not found or path is incorrect");
			return false;
		}
		return true;
	}

	bool LoadFonts() {
		if (!esrovar::mainfont.openFromFile("res/fonts/Roboto.ttf")) {
			LOG("Font not found or path is incorrect");
			return false;
		}
		return true;
	}

	std::tuple<int, int> getChunkXY(std::pair<float, float> playerxy) {
		float chunkX = std::floorf(playerxy.first / static_cast<float>(esrovar::CHUNK_SIZE * esrovar::pixel_size));
		float chunkY = std::floorf(playerxy.second / static_cast<float>(esrovar::CHUNK_SIZE * esrovar::pixel_size));
		
		return std::make_tuple(static_cast<int>(chunkX), static_cast<int>(chunkY));
	}

	std::tuple<int, int> getPlayerXY(std::pair<float, float> playerxy) {
		int playerX = static_cast<int>(std::floorf(playerxy.first / static_cast<float>(esrovar::pixel_size)));
		int playerY = static_cast<int>(std::floorf(playerxy.second / static_cast<float>(esrovar::pixel_size)));

		return std::make_tuple(playerX, playerY);
	}

	std::tuple<int, int> getPlayerChunkXY(std::pair<float, float> playerxy) {
		
		// Getting floored player coordinates
		float x = std::floor(playerxy.first);
		float y = std::floor(playerxy.second);
		const int chunkSizeInPixels = esrovar::CHUNK_SIZE * esrovar::pixel_size;
		
		// Adjusting for negative coordinates
		if (x < 0)
			x += std::abs(x * static_cast<float>(chunkSizeInPixels));
		if (y < 0)
			y += std::abs(y * static_cast<float>(chunkSizeInPixels));
		
		// Getting local chunk coordinates
		float chunkLocalX = std::fmod(x, static_cast<float>(chunkSizeInPixels));
		float chunkLocalY = std::fmod(y, static_cast<float>(chunkSizeInPixels));
		
		// Converting to chunk local coordinates
		int playerchunkX = static_cast<int>(std::floorf(chunkLocalX / static_cast<float>(esrovar::pixel_size)));
		int playerchunkY = static_cast<int>(std::floorf(chunkLocalY / static_cast<float>(esrovar::pixel_size)));
		
		return std::make_tuple(playerchunkX, playerchunkY);
	}
} // namespace esrofn ends

// Global objects and classes
namespace esroops {

	inline esroops::Tile& Chunk::tileAt(int x, int y) {
		const std::size_t idx = static_cast<std::size_t>(y) * static_cast<std::size_t>(esrovar::CHUNK_SIZE) + static_cast<std::size_t>(x);
		return tiles[idx];
	}

	inline const esroops::Tile& Chunk::tileAt(int x, int y) const {
		const std::size_t idx = static_cast<std::size_t>(y) * static_cast<std::size_t>(esrovar::CHUNK_SIZE) + static_cast<std::size_t>(x);
		return tiles[idx];
	}

	Chunk::Chunk(int x, int y) {
        // allocate contiguous storage for CHUNK_SIZE * CHUNK_SIZE tiles
        tiles.resize(static_cast<std::size_t>(esrovar::CHUNK_SIZE) * static_cast<std::size_t>(esrovar::CHUNK_SIZE));
		for (int yy = 0; yy < esrovar::CHUNK_SIZE; ++yy)
			for (int xx = 0; xx < esrovar::CHUNK_SIZE; ++xx) {
				tileAt(xx, yy).type = BlockType::plains;
			}
		m_chunkX = x;
		m_chunkY = y;
		m_tileset = esrovar::tileset;
		m_isGenerated = false;
	}

	Player::Player() {
		// Initializing member variables
		m_IsMoving = false;
		m_IsSlashAttacking = false;
		m_IsJumping = false;
		m_IsSitting = false;
		m_IsRunning = false;
		m_IsTileOcean = false;
		m_StateEnum = esrovar::State::idle;
		m_PrevStateEnum = esrovar::State::idle;
		m_DirectionEnum = esrovar::Directions::down;
		m_playerXY = sf::Vector2f(0.0f, 0.0f);
		m_TotalFrames = 0;
		m_CurrentFrame = 0;
		m_AnimTimer = 0.0f;
		m_AnimDuration = 0.0f;
		m_health = 500u;
		m_playersprite.emplace(esrovar::kTextures[esrovar::to_index(m_StateEnum)]);
		m_playersprite->setTextureRect(sf::IntRect({m_CurrentFrame * esrovar::PLAYER_SPRITE, static_cast<int>(esrovar::to_index(m_DirectionEnum)) * esrovar::PLAYER_SPRITE }, { esrovar::PLAYER_SPRITE, esrovar::PLAYER_SPRITE }));
		m_playersprite->setPosition({0, 0});
		m_playersprite->setScale({ 1.f, 1.f });
		setOrigintoBottomCenter();
	}

	WorldManager::WorldManager(std::pair<float, float> PLAYERXY, int seed) {
		// Initializing member variables
		m_playerchunk_X = PLAYERXY.first;
		m_playerchunk_Y = PLAYERXY.second;
		m_chunkGenerationLimit = 0;
		m_active_chunks;
		m_required_chunks;
		m_unrequired_chunks;
		m_world_seed = seed;
		m_tileColor = 0u;
		m_chunkframecounter = 0.0f;
		// Initializing member functions
		f_initialize_world();
	}

	Tile* Chunk::getTileData(int x, int y) {        
		return &tileAt(x, y);
	}

	void Chunk::generate(sf::Vector2f tilesize) {
		
		// Noise seed
		esrovar::noise.SetSeed(esrovar::gameseed);
		esrovar::noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
		esrovar::noise.SetFrequency(0.015f);
		esrovar::noise.SetDomainWarpType(FastNoiseLite::DomainWarpType::DomainWarpType_OpenSimplex2);
		esrovar::noise.SetDomainWarpAmp(10.0f);
		esrovar::noise.SetFractalType(FastNoiseLite::FractalType::FractalType_None);
		esrovar::noise.SetFractalOctaves(4);
		esrovar::noise.SetFractalLacunarity(2.0f);
		esrovar::noise.SetFractalGain(0.5f);
		
		// Set vertex properties
		m_grid.setPrimitiveType(sf::PrimitiveType::Triangles);
		m_grid.resize(static_cast<size_t>(esrovar::CHUNK_SIZE) * esrovar::CHUNK_SIZE * 6);
		
		// Instantiating vertex grid
		for (int y = 0; y < esrovar::CHUNK_SIZE; ++y) {
			for (int x = 0; x < esrovar::CHUNK_SIZE; ++x) {
				
				// tileIndex identifies where the tile will sit in this grid
				auto tileIndex = (x + y * esrovar::CHUNK_SIZE) * 6;
				
				// World Tile XY for noise logic
				auto worldTileX = static_cast<float>(m_chunkX * esrovar::CHUNK_SIZE + x);
				auto worldtileY = static_cast<float>(m_chunkY * esrovar::CHUNK_SIZE + y);
				
				// Getting noise value for current tile
				int noiseIDX = 0;				
				float lerp1 = esrovar::noise.GetNoise(worldTileX, worldtileY);
				float lerp2 = esrovar::noise.GetNoise(worldTileX + 0.7f, worldtileY + 0.3f);
				float noiseValue = lerp1 + 0.2f * (lerp2 - lerp1);
				noiseValue = (noiseValue + 1.0f) * 0.5f;
				
				if (noiseValue < 0.22) noiseIDX = 3; // Ocean
				else if (noiseValue < 0.28) noiseIDX = 1; // Beach
				else if (noiseValue < 0.45) noiseIDX = 4; // Plains
				else if (noiseValue < 0.65) noiseIDX = 0; // Forest
				else if (noiseValue < 0.85) noiseIDX = 2; // Dirt
				else noiseIDX = 5; // Mountain
				
				// Tile sheet index for now default selection is plains - Required in future implementations
				//auto tu = noiseIDX;
				//auto tv = 0;
				
				// Chunk pixel offset
				auto chunkOffsetX = static_cast<float>(m_chunkX * esrovar::CHUNK_SIZE) * tilesize.x;
				auto chunkOffsetY = static_cast<float>(m_chunkY * esrovar::CHUNK_SIZE) * tilesize.y;
				
				// XY of independent tile
				auto tx = chunkOffsetX + static_cast<float>(x) * tilesize.x;
				auto ty = chunkOffsetY + static_cast<float>(y) * tilesize.y;
				
				// XY for independent tile texture - Required in future implementations
				//auto texX = static_cast<float>(tu) * tilesize.x;
				//auto texY = static_cast<float>(tv) * tilesize.y;
				
				// Triangle 1
				m_grid[static_cast<size_t>(tileIndex) + 0].position = sf::Vector2f(tx, ty);
				m_grid[static_cast<size_t>(tileIndex) + 1].position = sf::Vector2f(tx + tilesize.x, ty);
				m_grid[static_cast<size_t>(tileIndex) + 2].position = sf::Vector2f(tx, ty + tilesize.y);
				m_grid[static_cast<size_t>(tileIndex) + 0].color = esrovar::biome_colors[noiseIDX];
				m_grid[static_cast<size_t>(tileIndex) + 1].color = esrovar::biome_colors[noiseIDX];
				m_grid[static_cast<size_t>(tileIndex) + 2].color = esrovar::biome_colors[noiseIDX];	
				// Triangle 2
				m_grid[static_cast<size_t>(tileIndex) + 3].position = sf::Vector2f(tx + tilesize.x, ty);
				m_grid[static_cast<size_t>(tileIndex) + 4].position = sf::Vector2f(tx + tilesize.x, ty + tilesize.y);
				m_grid[static_cast<size_t>(tileIndex) + 5].position = sf::Vector2f(tx, ty + tilesize.y);
				m_grid[static_cast<size_t>(tileIndex) + 3].color = esrovar::biome_colors[noiseIDX];
				m_grid[static_cast<size_t>(tileIndex) + 4].color = esrovar::biome_colors[noiseIDX];
				m_grid[static_cast<size_t>(tileIndex) + 5].color = esrovar::biome_colors[noiseIDX];
				
				// Setting tile's type at XY
				tileAt(x, y).type = static_cast<BlockType>(noiseIDX);
			}
		}
		
		// Updating chunk generated status		
		m_isGenerated = true;
	}

	void Chunk::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		states.transform *= getTransform();
		target.draw(m_grid, states);
	}    

	void Player::setOrigintoBottomCenter() {
		// Setting origin to bottom center of player sprite
		if (m_playersprite) {
			sf::FloatRect bounds = m_playersprite->getLocalBounds();
			m_playersprite->setOrigin({ bounds.size.x / 2.0f, bounds.size.y - 1 });
		}
	}

	void Player::update(float dt) {
		// Rendering and animating player sprite
		animatesprite(dt);
	}

	void Player::animatesprite(float dt) {
		
		// Flag check for player movement
		m_IsMoving = (esrovar::movedirx != 0 || esrovar::movediry != 0);
		
		// Setting animation duration based on player state
		m_AnimDuration = (m_IsMoving || m_IsJumping) ? 0.2f : 0.4f;
		
		// Setting player state, animation duration and handling other EDGE cases
		if (m_IsMoving) {
			m_StateEnum = esrovar::State::walk;
			m_IsSitting = false;
			if(m_IsRunning) {
				m_StateEnum = esrovar::State::run;
			}
			if(m_IsJumping) {
				m_StateEnum = esrovar::State::jump;
			}
		}
		else if (m_IsJumping) {
			m_StateEnum = esrovar::State::jump;
			m_IsSitting = false;
		}
		else if (m_IsSitting) {
			m_StateEnum = esrovar::State::sit;
		}
		else if (m_IsSlashAttacking) {
			m_StateEnum = esrovar::State::slash;
			m_IsSitting = false;
		}
		else {
			m_StateEnum = esrovar::State::idle;
			m_IsSitting = false;
		}
		
		// Resetting animation if there is change in player's state
		if (m_StateEnum != m_PrevStateEnum) {
			m_playersprite->setTexture(esrovar::kTextures[esrovar::to_index(m_StateEnum)]);
			m_PrevStateEnum = m_StateEnum;
			m_CurrentFrame = 0;
			m_AnimTimer = 0.0f;
		}		
		
		// Updating animation timing @ every frames
		m_AnimTimer += dt;
		
		// Checking player state
		if (m_StateEnum != esrovar::State::sit && m_StateEnum != esrovar::State::jump) {
			while (m_AnimTimer >= m_AnimDuration) {
				m_AnimTimer -= m_AnimDuration;
				m_CurrentFrame++;
				if (m_CurrentFrame >= esrovar::kFrameCount[esrovar::to_index(m_StateEnum)])
					m_CurrentFrame = 0;
			}
		}
		else if (m_StateEnum == esrovar::State::sit) {
				m_CurrentFrame = 0;
		}		
		else if (m_StateEnum == esrovar::State::jump) {
			while (m_AnimTimer >= m_AnimDuration) {
				m_AnimTimer -= m_AnimDuration;
				m_CurrentFrame++;
				if (m_CurrentFrame == 2)
					esrovar::jumpboost = 150;
				if (m_CurrentFrame >= esrovar::kFrameCount[esrovar::to_index(m_StateEnum)]) {
					m_CurrentFrame = 0;
					m_IsJumping = false;
					esrovar::jumpboost = 0;
				}
			}
		}
		
		// Updating texture rectangle
		m_playersprite->setTextureRect( sf::IntRect({ m_CurrentFrame * esrovar::PLAYER_SPRITE, static_cast<int>(esrovar::to_index(m_DirectionEnum)) * esrovar::PLAYER_SPRITE }, { esrovar::PLAYER_SPRITE, esrovar::PLAYER_SPRITE } ));
		//m_playersprite->setPosition(m_playerXY);
	}

	void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		// Draws player object
		states.transform *= getTransform();
		target.draw(*m_playersprite, states);
	}

	void WorldManager::f_initialize_world() {
		
		getRequiredChunks();
		// Initializing set to hold required chunk in that frame
		if (!m_required_chunks.empty() && !m_active_chunks.contains({m_required_chunks.front()})) {
			Chunk _chunk(m_required_chunks.front().first, m_required_chunks.front().second);
			_chunk.generate(sf::Vector2f({ static_cast<float>(esrovar::pixel_size), static_cast<float>(esrovar::pixel_size) }));
			m_active_chunks.try_emplace({ m_required_chunks.front() }, _chunk);
			m_required_chunks.pop_front();
		}
	}

	void WorldManager::getRequiredChunks() {
		
		// Clearing previous frame's required and unrequired chunks
		m_required_chunks.clear();
		m_unrequired_chunks.clear();
		
		// Function to calculate required chunks around player
		for (int x = -esrovar::CHUNK_RADIUS; x <= esrovar::CHUNK_RADIUS; ++x) {
			for (int y = -esrovar::CHUNK_RADIUS; y <= esrovar::CHUNK_RADIUS; ++y) {
				int targetX = static_cast<int>(m_playerchunk_X) + x;
				int targetY = static_cast<int>(m_playerchunk_Y) + y;
				if(!m_active_chunks.contains({ targetX, targetY }))
					m_required_chunks.push_back({ targetX, targetY });
			}
		}
		// Function to calculate unrequired chunks outside player's radius
		for (const auto& [coords, _chunk] : m_active_chunks) {
			auto& [cx, cy] = coords;
			if (std::abs(cx - m_playerchunk_X) > esrovar::CHUNK_RADIUS || std::abs(cy - m_playerchunk_Y) > esrovar::CHUNK_RADIUS) {
				m_unrequired_chunks.push_back({ cx, cy });
			}
		}
	}

	void WorldManager::update(float dt) {

		// Need to create logic to get one chunk out from requiredchunk array
		if (!m_required_chunks.empty() && !m_active_chunks.contains({ m_required_chunks.front() })) {
			Chunk _chunk(m_required_chunks.front().first, m_required_chunks.front().second);
			_chunk.generate(sf::Vector2f({ static_cast<float>(esrovar::pixel_size), static_cast<float>(esrovar::pixel_size) }));
			m_active_chunks.try_emplace({m_required_chunks.front()}, _chunk);
			m_required_chunks.pop_front();
		}
		// Remove one chunk out from unrequiredchunk array
		if (!m_unrequired_chunks.empty()) {
			m_active_chunks.at(m_unrequired_chunks.front()).m_isGenerated = false;
			m_active_chunks.erase(m_unrequired_chunks.front());
			m_unrequired_chunks.pop_front();
		}
	}

	void WorldManager::f_drawChunks(sf::RenderWindow& window) const {
		for (auto& [coords, _chunk] : m_active_chunks) {
			window.draw(_chunk);
		}
	}

	void WorldManager::ChunkBorders(sf::RenderWindow& window) const {
		// Drawing chunk borders for debugging
		for (int x = -esrovar::CHUNK_RADIUS; x <= esrovar::CHUNK_RADIUS; ++x) {
			for (int y = -esrovar::CHUNK_RADIUS; y <= esrovar::CHUNK_RADIUS; ++y) {
				int targetX = static_cast<int>(m_playerchunk_X) + x;
				int targetY = static_cast<int>(m_playerchunk_Y) + y;
				sf::RectangleShape rectangle;
				rectangle.setSize(sf::Vector2f(static_cast<float>(esrovar::chunk_area), static_cast<float>(esrovar::chunk_area)));
				rectangle.setFillColor(sf::Color::Transparent);
				rectangle.setOutlineColor(sf::Color::Red);
				rectangle.setOutlineThickness(1.0f);
				rectangle.setPosition(sf::Vector2f(static_cast<float>(targetX * esrovar::chunk_area), static_cast<float>(targetY * esrovar::chunk_area)));
				window.draw(rectangle);
			}
		}
	}

	std::string WorldManager::getTileType(std::pair<float, float> playerXY) {
		
		// Getting chunk coordinates from player coordinates
		auto [chunkX, chunkY] = esrofn::getChunkXY(playerXY);
		
		// Getting player local chunk coordinates
		auto [playerchunkX, playerchunkY] = esrofn::getPlayerChunkXY(playerXY);
		
		// Fetching tile data from active chunks
		if (m_active_chunks.contains({ chunkX, chunkY })) {
			Chunk& currentChunk = m_active_chunks.at({ chunkX, chunkY });
			const Tile* tile = currentChunk.getTileData(static_cast<int>(playerchunkX), static_cast<int>(playerchunkY));
			if (tile) {
				return tilevariation[static_cast<int>(tile->type)];
			}
		}
		// Tile data not found case
		return std::format("Tile data not found at ({}, {}) in chunk ({}, {})", playerchunkX, playerchunkY, chunkX, chunkY);
	}
} // namespace esroops ends