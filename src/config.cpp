// Including necessary libraries
#include "headers/config.hpp"
#include "headers/chunkManager.hpp"
#include <cmath>
#include <cstdint>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Vector2.hpp>
#include <map>
#include <string>
#include <utility>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <ranges>
#include <array>
#include <cstdlib>
#include <format>
#include <tuple>

// Global variables
namespace esrovar {

	float speed = 100.0f;
	float movedirx = 0.0f;
	float movediry = 0.0f;
	float boost = 0.0f;
	float dt = 0.0f;
	float totalspeed = 0.0f;
	int jumpboost = 0;
	bool ChunkBorder = false;
	bool DebugMode = false;
	bool Save = false;
	std::string playerFileURI = "res\\user_dat\\";
	std::string saveFile = "data.json";
	int frame_count = 0;
	sf::RenderWindow GameWindow;
	std::pair<float, float> PLAYER_POSITION = {0.0f, 0.0f};
	std::array<sf::Texture, StateCount> kTextures;
	sf::Font mainfont;
	std::ranges::iota_view<int, int> centerPOS = std::views::iota(-CHUNK_RADIUS, CHUNK_RADIUS + 1);
} // namespace esrovar ends

// Global objects and classes
namespace esroops {

	WorldManager::WorldManager(std::pair<float, float> PLAYERXY, uint32_t seed) {
		// Initializing member variables
		m_playerchunk_X = PLAYERXY.first;
		m_playerchunk_Y = PLAYERXY.second;
		m_chunkGenerationLimit = 2;
		m_active_chunks;
		m_required_chunks;
		m_unrequired_chunks;
		m_world_seed = seed;
		m_tileColor = 0u;		
		f_initialize_world();
	}

	void WorldManager::f_initialize_world() {
		
		// Layers for map noise generation are initiated
		initElevationLayer(m_world_seed);
		
		// Get initial chunks required around player
		getRequiredChunks();
		
		// Initializing set to hold required chunk in that frame
		if (!m_required_chunks.empty() && !m_active_chunks.contains({m_required_chunks.front()})) {
			Chunk _chunk(m_required_chunks.front().first, m_required_chunks.front().second);
			_chunk.generate(sf::Vector2f({ static_cast<float>(pixel_size), static_cast<float>(pixel_size) }), m_world_seed);
			m_active_chunks.try_emplace({ m_required_chunks.front() }, _chunk);
			m_required_chunks.pop_front();
		}
	}

	void WorldManager::getRequiredChunks() {
		
		// Clearing earlier required and unrequired chunks
		m_required_chunks.clear();
		m_unrequired_chunks.clear();
		
		// Function to calculate required chunks around player
		for (int x : esrovar::centerPOS) {
			for (int y : esrovar::centerPOS) {
				int targetX = static_cast<int>(m_playerchunk_X) + x;
				int targetY = static_cast<int>(m_playerchunk_Y) + y;
				if(!m_active_chunks.contains({ targetX, targetY }))
					m_required_chunks.push_back({ targetX, targetY });
			}
		}
		// Function to calculate unrequired chunks outside player's radius
		for (const auto& [coords, _chunk] : m_active_chunks) {
			auto& [cx, cy] = coords;
			if (std::abs(cx - m_playerchunk_X) > CHUNK_RADIUS || std::abs(cy - m_playerchunk_Y) > CHUNK_RADIUS) {
				m_unrequired_chunks.push_back({ cx, cy });	
			}
		}
	}

	void WorldManager::update(float dt) {
		
		// Need to create logic to get one chunk out from requiredchunk array
		// Check if required chunks deque is not empty
		if (!m_required_chunks.empty()) {
			// Reference variable for first chunk in m_required_chunks
			const auto& firstChunk = m_required_chunks.front();
			// Check if active chunks pair does not contains targeted required chunk
			if(!m_active_chunks.contains(firstChunk))
			{
				// Init new Chunk
				Chunk _chunk(firstChunk.first, firstChunk.second);
				// Generate initialized chunk
				_chunk.generate(sf::Vector2f( static_cast<float>(pixel_size), static_cast<float>(pixel_size)), m_world_seed);
				// Add generated chunk to active chunks registry
				m_active_chunks.try_emplace(firstChunk, std::move(_chunk));
			}
			// Remove one chunk out from requiredchunk array
			m_required_chunks.pop_front();
		}
		
		// Remove one chunk out from unrequiredchunk array
		if (!m_unrequired_chunks.empty()) {
			const auto& key = m_unrequired_chunks.front();
			auto it = m_active_chunks.find(key);
			if (it != m_active_chunks.end()) {
				m_active_chunks.erase(it);
			}
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
		for (int x : esrovar::centerPOS) {
			for (int y : esrovar::centerPOS) {
				sf::RectangleShape rectangle;
				int targetX = static_cast<int>(m_playerchunk_X) + x;
				int targetY = static_cast<int>(m_playerchunk_Y) + y;
				rectangle.setSize(sf::Vector2f(static_cast<float>(chunk_area), static_cast<float>(chunk_area)));
				rectangle.setFillColor(sf::Color::Transparent);
				rectangle.setOutlineColor(sf::Color::Red);
				rectangle.setOutlineThickness(1.0f);
				rectangle.setPosition(sf::Vector2f(static_cast<float>(targetX * chunk_area), static_cast<float>(targetY * chunk_area)));
				window.draw(rectangle);
			}			
		}
	}

	std::string WorldManager::getTileType(const std::pair<float, float>& playerXY) const {

		// Getting chunk coordinates from player coordinates
		auto [chunkX, chunkY] = getChunkXY(playerXY);

		// Getting player local chunk coordinates
		auto [playerchunkX, playerchunkY] = getPlayerChunkXY(playerXY);
		
		// Checking active chunks existance and fetching memory address
		auto it = m_active_chunks.find({ chunkX, chunkY });
		
		// Check if the chunk exists in the active chunks map
		if (it != m_active_chunks.end()) {
			
			// Access the chunk and get the tile data
			const Chunk& currentChunk = it->second;
			// Get the tile data at the player's local chunk coordinates
			const Tile& tile = currentChunk.tileAt(static_cast<int>(playerchunkX), static_cast<int>(playerchunkY));
			
			// Check if the tile data is valid and return the corresponding tile type string
			if (&tile) {
				// Return the string representation of the tile type using the tilevariation array
				return tilevariation[static_cast<int>(tile.type)];
			}
		}
		// Tile data not found case
		return std::format("Tile data not found at ({}, {}) in chunk ({}, {})", playerchunkX, playerchunkY, chunkX, chunkY);
	}

	void WorldManager::initElevationLayer(uint32_t& seed) {
		elevationNoise.SetSeed(seed);
		elevationNoise.SetFractalOctaves(4);
		elevationNoise.SetFractalGain(0.5f);
		elevationNoise.SetFrequency(0.015f);
		elevationNoise.SetDomainWarpAmp(10.0f);
		elevationNoise.SetFractalLacunarity(2.0f);
		elevationNoise.SetFractalType(FastNoiseLite::FractalType::FractalType_None);
		elevationNoise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S);
		elevationNoise.SetDomainWarpType(FastNoiseLite::DomainWarpType::DomainWarpType_OpenSimplex2);
	}
} // namespace esroops ends