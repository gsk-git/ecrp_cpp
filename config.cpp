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
	int chunk_area = CHUNK_SIZE * pixel_size;
	float movedirx = 0.0f;
	float movediry = 0.0f;
	float boost = 0.0f;
	float dt = 0.0f;
	float totalspeed = 0.0f;
	int jumpboost = 0;
	sf::VideoMode desktop = desktop.getDesktopMode();
	sf::RenderWindow GameWindow(desktop, "ESRO", sf::Style::None);
	std::pair<int, int> PLAYER_POSITION = {0, 0};
	std::array<sf::Texture, StateCount> kTextures;
} // namespace esrovar ends

// Global functions
namespace esrofn {

	bool LoadSpriteSheetsnew() {
		for (std::size_t i = 0; i < esrovar::StateCount; ++i) {
			// Convert string_view → std::string for SFML
			std::string path{ esrovar::kTexturePaths[i] };
			if (!esrovar::kTextures[i].loadFromFile(path)) {
				LOG("Failed to load texture: " + path);
				return false;
			}
		}
		return false;
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
		m_IsSlashAttacking = false;
		m_IsJumping = false;
		m_IsSitting = false;
		m_IsRunning = false;
		m_StateEnum = esrovar::State::idle;
		m_PrevStateEnum = esrovar::State::idle;
		m_DirectionEnum = esrovar::Directions::down;
		m_playerXY = sf::Vector2f(static_cast<float>(0.0f), static_cast<float>(0.0f));
		m_TotalFrames = 0;
		m_CurrentFrame = 0;
		m_AnimTimer = 0.0f;
		m_AnimDuration = 0.0f;
		m_health = 500u;
		//m_playersprite.emplace(esrovar::TextureFace[m_State]);
		m_playersprite.emplace(esrovar::kTextures[esrovar::to_index(m_StateEnum)]);
		m_playersprite->setTextureRect(sf::IntRect({ m_CurrentFrame * esrovar::PLAYER_SPRITE, static_cast<int>(esrovar::to_index(m_DirectionEnum)) * esrovar::PLAYER_SPRITE }, { esrovar::PLAYER_SPRITE, esrovar::PLAYER_SPRITE }));
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
		// Rendering and animating player sprite
		animatesprite(dt);
	}

	void Player::animatesprite(float dt) {
		
		// Flag check for player movement
		m_IsMoving = (esrovar::movedirx != 0 || esrovar::movediry != 0);
		m_AnimDuration = 0.2f;
		
		// Setting player state and animation duration
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
		
		// Resetting animation if state changed
		if (m_StateEnum != m_PrevStateEnum) {
			m_playersprite->setTexture(esrovar::kTextures[esrovar::to_index(m_StateEnum)]);
			m_PrevStateEnum = m_StateEnum;
			m_CurrentFrame = 0;
			m_AnimTimer = 0.0f;
		}		
		// Updating animation timing
		m_AnimTimer += dt;
		
		// Checking whether player is in sitting state or jumping state
		if (m_StateEnum != esrovar::State::sit && m_StateEnum != esrovar::State::jump) {
			while (m_AnimTimer >= m_AnimDuration) {
				m_AnimTimer -= m_AnimDuration;
				m_CurrentFrame++;
				if (m_CurrentFrame >= esrovar::kFrameCount[esrovar::to_index(m_StateEnum)])
					m_CurrentFrame = 0;
			}
		}
		// Special case for sitting state
		else if (m_StateEnum == esrovar::State::sit) {
				m_CurrentFrame = 0;
		}
		// Special case for jumping state
		else if (m_StateEnum == esrovar::State::jump) {
			while (m_AnimTimer >= m_AnimDuration) {
				m_AnimTimer -= m_AnimDuration;
				m_CurrentFrame++;
				if (m_CurrentFrame >= esrovar::kFrameCount[esrovar::to_index(m_StateEnum)]) {
					m_CurrentFrame = 0;
					m_IsJumping = false;
					esrovar::jumpboost = 0;
				}
			}
		}
		
		// Updating texture rectangle
		m_playersprite->setTextureRect(sf::IntRect({ m_CurrentFrame * esrovar::PLAYER_SPRITE, static_cast<int>(esrovar::to_index(m_DirectionEnum)) * esrovar::PLAYER_SPRITE }, { esrovar::PLAYER_SPRITE, esrovar::PLAYER_SPRITE }));
		m_playersprite->setPosition(m_playerXY);
	}

	void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		// Draws player object
		states.transform *= getTransform();
		target.draw(*m_playersprite, states);
	}

	void WorldManager::f_initialize_world() {
		// Logging world seed
		LOG("Initializing world .....");
		LOG("Initializing World Seed: " + std::to_string(m_world_seed));
		LOG("Placing player on Chunk Coordinates: (" + std::to_string(m_playerchunk_X) + ", " + std::to_string(m_playerchunk_Y) + ")");
		LOG("Generating chunks around player within radius: " + std::to_string(esrovar::CHUNK_RADIUS));
		LOG("Each chunk is of size: " + std::to_string(esrovar::CHUNK_SIZE) + "x" + std::to_string(esrovar::CHUNK_SIZE) + " tiles");
		
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
				Chunk _chunk(cx, cy);
				_chunk.generate("res/tile.png", sf::Vector2i({esrovar::pixel_size, esrovar::pixel_size }));
				m_active_chunks.insert({{cx, cy}, _chunk});
			}
		}
		LOG("Active chunks " << m_active_chunks.size());
		LOG("World initialization complete.");
	}

	void WorldManager::f_drawChunks(sf::RenderWindow& window) {
		for (auto& [coords, _chunk] : m_active_chunks) {
			window.draw(_chunk);
		}
	}
} // namespace esroops ends