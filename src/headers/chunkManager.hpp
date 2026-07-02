#pragma once

#include <FastNoise/include/FastNoise/FastNoiseLite.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <cmath>
#include <tuple>
#include <utility>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

#ifndef CONFIG_HPP_LOG_MACRO
#define CONFIG_HPP_LOG_MACRO

#include <sstream>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#include <tuple>
#define LOG(x) do { \
    std::ostringstream _oss; _oss << (x); \
    std::string _s = _oss.str(); _s.push_back('\n'); \
    OutputDebugStringA(_s.c_str()); \
} while (0)
#else
#define LOG(x) do { \
    std::ostringstream _oss; _oss << (x); \
    std::cerr << _oss.str() << '\n'; \
} while (0)
#endif

#endif

constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_RADIUS = 2;
constexpr int pixel_size = 30;
inline sf::Texture tileset;
inline sf::RenderWindow GameWindow;
extern FastNoiseLite elevationNoise;
constexpr int chunk_area = CHUNK_SIZE * pixel_size;
extern std::vector<sf::Color> biome_colors;
extern std::vector<sf::Color> biome_variants;

inline std::tuple<int, int> getChunkXY(std::pair<float, float> playerxy) {
	float chunkX = std::floorf(playerxy.first / static_cast<float>(CHUNK_SIZE * pixel_size));
	float chunkY = std::floorf(playerxy.second / static_cast<float>(CHUNK_SIZE * pixel_size));

		return std::make_tuple(static_cast<int>(chunkX), static_cast<int>(chunkY));
}

inline std::tuple<int, int> getPlayerChunkXY(std::pair<float, float> playerxy) {

	// Getting floored player coordinates
	float x = std::floor(playerxy.first);
	float y = std::floor(playerxy.second);
	const int chunkSizeInPixels = CHUNK_SIZE * pixel_size;

	// Adjusting for negative coordinates
	if (x < 0)
		x += std::abs(x * static_cast<float>(chunkSizeInPixels));
	if (y < 0)
		y += std::abs(y * static_cast<float>(chunkSizeInPixels));

	// Getting local chunk coordinates
	float chunkLocalX = std::fmod(x, static_cast<float>(chunkSizeInPixels));
	float chunkLocalY = std::fmod(y, static_cast<float>(chunkSizeInPixels));

	// Converting to chunk local coordinates
	int playerchunkX = static_cast<int>(std::floorf(chunkLocalX / static_cast<float>(pixel_size)));
	int playerchunkY = static_cast<int>(std::floorf(chunkLocalY / static_cast<float>(pixel_size)));

	return std::make_tuple(playerchunkX, playerchunkY);
}

enum class BlockType {
    plains,
    beach,
    dirt,
    ocean,
    forest,
    mountain
};

struct Tile {
    BlockType type;
};

class Chunk : public sf::Drawable, public sf::Transformable {
public:
    Chunk() = default;
    Chunk(int x, int y);
    ~Chunk() final = default;
    int m_chunkX;
    int m_chunkY;
    bool m_isGenerated;
    std::vector<Tile> tiles;
    Tile& tileAt(int x, int y);
    const Tile& tileAt(int x, int y) const;
    sf::VertexArray m_grid;
    sf::Texture m_tileset;
    // Member functions
    void generate(sf::Vector2f tilesize, uint32_t worldseed);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    bool getContinentLayer(int x, int y);
};

inline Tile& Chunk::tileAt(int x, int y) {
	const std::size_t idx = static_cast<std::size_t>(y) * static_cast<std::size_t>(CHUNK_SIZE) + static_cast<std::size_t>(x);
	return tiles[idx];
}

inline const Tile& Chunk::tileAt(int x, int y) const {
	const std::size_t idx = static_cast<std::size_t>(y) * static_cast<std::size_t>(CHUNK_SIZE) + static_cast<std::size_t>(x);
	return tiles[idx];
}

inline Chunk::Chunk(int x, int y) {
	// allocate contiguous storage for CHUNK_SIZE * CHUNK_SIZE tiles
	tiles.resize(static_cast<std::size_t>(CHUNK_SIZE) * static_cast<std::size_t>(CHUNK_SIZE));
	for (int yy = 0; yy < CHUNK_SIZE; ++yy)
		for (int xx = 0; xx < CHUNK_SIZE; ++xx) {
			tileAt(xx, yy).type = BlockType::plains;
		}
	m_chunkX = x;
	m_chunkY = y;
	m_tileset = tileset;
	m_isGenerated = false;
}

inline bool Chunk::getContinentLayer(int x, int y) {
	// Getting continent noise value
	float worldTileX = static_cast<float>(x * CHUNK_SIZE);
	float worldTileY = static_cast<float>(y * CHUNK_SIZE);
	float noiseValue = elevationNoise.GetNoise(worldTileX * 0.005f, worldTileY * 0.005f);
	noiseValue = (noiseValue + 1.0f) * 0.5f;
	// Determining land or ocean based on noise value
	return noiseValue > 0.4f; // Threshold for land
}

inline void Chunk::generate(sf::Vector2f tilesize, uint32_t seed) {

	// Get baseMap logic
	bool isLand = getContinentLayer(m_chunkX, m_chunkY);

	// Set vertex properties
	m_grid.setPrimitiveType(sf::PrimitiveType::Triangles);
	m_grid.resize(static_cast<size_t>(CHUNK_SIZE) * CHUNK_SIZE * 6);

	std::size_t tileIndex;
	float worldTileX;
	float worldtileY;
	int noiseIDX;
	float lerp1;
	float lerp2;
	float noiseValue;
	float chunkOffsetX = m_chunkX * CHUNK_SIZE * tilesize.x;
	float chunkOffsetY = m_chunkY * CHUNK_SIZE * tilesize.y;
	float tx;
	float ty;

	// Instantiating vertex grid
	for (int y = 0; y < CHUNK_SIZE; ++y) {
		for (int x = 0; x < CHUNK_SIZE; ++x) {

			// tileIndex identifies where the tile will sit in this grid
			tileIndex = (x + y * CHUNK_SIZE) * 6;

			// World Tile XY for noise logic
			worldTileX = static_cast<float>(m_chunkX * CHUNK_SIZE + x);
			worldtileY = static_cast<float>(m_chunkY * CHUNK_SIZE + y);

			// Getting baseNoise value for current tile
			noiseIDX = 0;
			lerp1 = elevationNoise.GetNoise(worldTileX, worldtileY);
			lerp2 = elevationNoise.GetNoise(worldTileX + 0.7f, worldtileY + 0.3f);
			noiseValue = lerp1 + 0.2f * (lerp2 - lerp1);
			noiseValue = (noiseValue + 1.0f) * 0.5f;

			// Determining baseBiome based on baseNoise value
			if (isLand) {
				if (noiseValue < 0.22) noiseIDX = 3;		// lake
				else if (noiseValue < 0.28) noiseIDX = 1;	// Beach
				else if (noiseValue < 0.45) noiseIDX = 0;	// Plains -> Forest
				else if (noiseValue < 0.65) noiseIDX = 4;	// Forest -> Plains
				else if (noiseValue < 0.85) noiseIDX = 2;	// Dirt
				else noiseIDX = 5;							// Mountain
			}
			else noiseIDX = 3;								// Ocean

			// XY of independent tile
			tx = chunkOffsetX + x * tilesize.x;
			ty = chunkOffsetY + y * tilesize.y;

			// XY for independent tile texture - Required in future implementations
			//auto texX = static_cast<float>(tu) * tilesize.x;
			//auto texY = static_cast<float>(tv) * tilesize.y;
			// Tile sheet index for now default selection is plains - Required in future implementations
			//auto tu = noiseIDX;
			//auto tv = 0;

			// Triangle 1
			m_grid[tileIndex + 0].position = sf::Vector2f(tx, ty);
			m_grid[tileIndex + 1].position = sf::Vector2f(tx + tilesize.x, ty);
			m_grid[tileIndex + 2].position = sf::Vector2f(tx, ty + tilesize.y);
			m_grid[tileIndex + 0].color = biome_colors[noiseIDX];
			m_grid[tileIndex + 1].color = biome_colors[noiseIDX];
			m_grid[tileIndex + 2].color = biome_colors[noiseIDX];
			// Triangle 2
			m_grid[tileIndex + 3].position = sf::Vector2f(tx + tilesize.x, ty);
			m_grid[tileIndex + 4].position = sf::Vector2f(tx + tilesize.x, ty + tilesize.y);
			m_grid[tileIndex + 5].position = sf::Vector2f(tx, ty + tilesize.y);
			m_grid[tileIndex + 3].color = biome_colors[noiseIDX];
			m_grid[tileIndex + 4].color = biome_colors[noiseIDX];
			m_grid[tileIndex + 5].color = biome_colors[noiseIDX];

			// Setting tile's type at XY
			tileAt(x, y).type = static_cast<BlockType>(noiseIDX);
		}
	}
	// Updating chunk generated status		
	m_isGenerated = true;
}

inline void Chunk::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(m_grid, states);
}




