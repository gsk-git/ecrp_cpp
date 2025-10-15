// Including necessary libraries
#include "config.hpp"
#include <iostream>
#include <cmath>
#include <random>
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
	FastNoiseLite noise;
	std::pair<float, float> PLAYER_POSITION = {0.0f, 0.0f};
	std::array<sf::Texture, StateCount> kTextures;
	sf::Font mainfont;
	sf::Texture tileset;
	bool ChunkBorder = false;
	bool DebugMode = false;
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
				LOG("Failed to load texture: " + path);
				return false;
			}
		}
		return false;
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
		float chunkX = std::floorf(playerxy.first / (esrovar::CHUNK_SIZE * esrovar::pixel_size));
		float chunkY = std::floorf(playerxy.second / (esrovar::CHUNK_SIZE * esrovar::pixel_size));
		
		return std::make_tuple(static_cast<int>(chunkX), static_cast<int>(chunkY));
	}

	std::tuple<int, int> getPlayerXY(std::pair<float, float> playerxy) {
		float playerX = std::floorf(playerxy.first / esrovar::pixel_size);
		float playerY = std::floorf(playerxy.second / esrovar::pixel_size);

		return std::make_tuple(static_cast<int>(playerX), static_cast<int>(playerY));
	}

	std::tuple<float, float> getPlayerChunkXY(std::pair<float, float> playerxy) {
		
		// Getting floored player coordinates
		float x = std::floor(playerxy.first);
		float y = std::floor(playerxy.second);
		const int chunkSizeInPixels = esrovar::CHUNK_SIZE * esrovar::pixel_size;
		
		// Adjusting for negative coordinates
		if (x < 0)
			x += std::abs(x * chunkSizeInPixels);
		if (y < 0)
			y += std::abs(y * chunkSizeInPixels);
		
		// Getting local chunk coordinates
		float chunkLocalX = std::fmod(x, static_cast<float>(chunkSizeInPixels));
		float chunkLocalY = std::fmod(y, static_cast<float>(chunkSizeInPixels));
		
		// Converting to chunk local coordinates
		float playerchunkX = std::floorf(chunkLocalX / esrovar::pixel_size);
		float playerchunkY = std::floorf(chunkLocalY / esrovar::pixel_size);
		
		return std::make_tuple(playerchunkX, playerchunkY);
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
		m_StateEnum = esrovar::State::idle;
		m_PrevStateEnum = esrovar::State::idle;
		m_DirectionEnum = esrovar::Directions::down;
		m_playerXY = sf::Vector2f(static_cast<float>(0.0f), static_cast<float>(0.0f));
		m_TotalFrames = 0;
		m_CurrentFrame = 0;
		m_AnimTimer = 0.0f;
		m_AnimDuration = 0.0f;
		m_health = 500u;
		m_playersprite.emplace(esrovar::kTextures[esrovar::to_index(m_StateEnum)]);
		m_playersprite->setTextureRect(sf::IntRect({ 
			m_CurrentFrame * esrovar::PLAYER_SPRITE, 
			static_cast<int>(esrovar::to_index(m_DirectionEnum)) * esrovar::PLAYER_SPRITE }, 
			{ esrovar::PLAYER_SPRITE, esrovar::PLAYER_SPRITE }));
		m_playersprite->setPosition(m_playerXY);
		setOrigintoBottomCenter();
	}

	WorldManager::WorldManager(std::pair<int, int> PLAYERXY, int seed) {    
		m_playerchunk_X = PLAYERXY.first;
		m_playerchunk_Y = PLAYERXY.second;
		m_active_chunks;
		m_world_seed = seed;
		m_chunkframecounter = 0;
		f_initialize_world();
	}

	Tile* Chunk::getTileData(int x, int y) {        
		return &tiles[x][y];
	}

	void Chunk::generate(sf::Vector2i tilesize, int Color) {
		
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
				int tu = Color; // Random tile selection
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
				if (Color == 0)
					tiles[x][y].type = BlockType::plains;
				else if (Color == 1)
					tiles[x][y].type = BlockType::beach;
				else if (Color == 2)
					tiles[x][y].type = BlockType::dirt;
				else if (Color == 3)
					tiles[x][y].type = BlockType::ocean;
				else if (Color == 4)
					tiles[x][y].type = BlockType::forest;
				else if (Color == 5)
					tiles[x][y].type = BlockType::mountain;
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
		// Special animation for sitting state
		else if (m_StateEnum == esrovar::State::sit) {
				m_CurrentFrame = 0;
		}
		// Special animation for jumping state
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
		m_playersprite->setTextureRect(
			sf::IntRect({ m_CurrentFrame * esrovar::PLAYER_SPRITE, 
			static_cast<int>(esrovar::to_index(m_DirectionEnum)) * esrovar::PLAYER_SPRITE }, 
			{ esrovar::PLAYER_SPRITE, esrovar::PLAYER_SPRITE }));
		m_playersprite->setPosition(m_playerXY);
	}

	void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
		// Draws player object
		states.transform *= getTransform();
		target.draw(*m_playersprite, states);
	}

	void WorldManager::f_initialize_world() {
		
		int color = 0;
		// Initializing set to hold required chunk in that frame
		for (int x = -esrovar::CHUNK_RADIUS; x <= esrovar::CHUNK_RADIUS; ++x) {
			for (int y = -esrovar::CHUNK_RADIUS; y <= esrovar::CHUNK_RADIUS; ++y) {
				int targetX = m_playerchunk_X + x;
				int targetY = m_playerchunk_Y + y;
				
				if (!m_active_chunks.contains({ targetX, targetY })) {
					Chunk _chunk(targetX, targetY);
					color = esrovar::DebugMode ? m_world_seed : 0;
					_chunk.generate(sf::Vector2i({ esrovar::pixel_size, esrovar::pixel_size }), color);
					m_active_chunks.insert({ { targetX, targetY }, _chunk });
				}
			}
		}
	}

	void WorldManager::update(int seed) {		
		
		// Updating chunk frame counter
		if (m_chunkframecounter % 10 != 0) return;
		else ++m_chunkframecounter;
		
		// Initializing required variables
		int color = 0;
		
		// Calculating visible chunk radius around player
		for (int x = -esrovar::CHUNK_RADIUS; x <= esrovar::CHUNK_RADIUS; ++x) {
			for (int y = -esrovar::CHUNK_RADIUS; y <= esrovar::CHUNK_RADIUS; ++y) {
				int targetX = m_playerchunk_X + x;
				int targetY = m_playerchunk_Y + y;
				
				if (!m_active_chunks.contains({ targetX, targetY })) {
					Chunk _chunk(targetX, targetY);
					color = esrovar::DebugMode ? seed : 0;
					_chunk.generate(sf::Vector2i({ esrovar::pixel_size, esrovar::pixel_size }), color);
					m_active_chunks.insert({ { targetX, targetY }, _chunk });
				}
			}
		}
		
		// Removing chunks which are out of player's radius and marking chunk m_generated to false
		for (auto it = m_active_chunks.begin(); it != m_active_chunks.end(); ) {
			auto [cx, cy] = it->first;
			if (std::abs(cx - m_playerchunk_X) > esrovar::CHUNK_RADIUS || std::abs(cy - m_playerchunk_Y) > esrovar::CHUNK_RADIUS) {
				m_active_chunks.at({ cx,cy }).m_isGenerated = false;
				it = m_active_chunks.erase(it);
			} else {
				++it;
			}
		}
		
		// Resetting chunk frame counter
		m_chunkframecounter = 0;
	}

	void WorldManager::f_drawChunks(sf::RenderWindow& window) {
		for (auto& [coords, _chunk] : m_active_chunks) {
			window.draw(_chunk);
		}
	}

	void WorldManager::ChunkBorders(sf::RenderWindow& window) const {
		// Drawing chunk borders for debugging
		for (int x = -esrovar::CHUNK_RADIUS; x <= esrovar::CHUNK_RADIUS; ++x) {
			for (int y = -esrovar::CHUNK_RADIUS; y <= esrovar::CHUNK_RADIUS; ++y) {
				int targetX = m_playerchunk_X + x;
				int targetY = m_playerchunk_Y + y;
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

	std::string WorldManager::getTileType(float x, float y) {
		
		// Getting chunk coordinates from player coordinates
		auto [chunkX, chunkY] = esrofn::getChunkXY({ x,y });
		
		// Getting player local chunk coordinates
		auto [playerchunkX, playerchunkY] = esrofn::getPlayerChunkXY({ x,y });
		
		// Fetching tile data from active chunks
		if (m_active_chunks.contains({ chunkX, chunkY })) {
			Chunk& currentChunk = m_active_chunks.at({ chunkX, chunkY });
			Tile* tile = currentChunk.getTileData(static_cast<int>(playerchunkX), static_cast<int>(playerchunkY));
			if (tile) {
				return tilevariation[static_cast<int>(tile->type)];
			}
			else {
				return std::format("Tile data not found at ({}, {}) in chunk ({}, {})", playerchunkX, playerchunkY, chunkX, chunkY);
			}
		}
	}
} // namespace esroops ends