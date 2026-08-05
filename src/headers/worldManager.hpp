#pragma once

// Including all necessary libraries
#include <map>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <utility>
#include <deque>
#include <SFML/Graphics/Rect.hpp>
#include "chunkManager.hpp"

class WorldManager {
public:
    // Member variables
    WorldManager() = default;
    WorldManager(std::pair<float, float>, uint32_t seed);
    ~WorldManager() = default;
    float m_playerchunk_X;
    float m_playerchunk_Y;
    int m_chunkGenerationLimit;
    uint32_t m_world_seed;
    unsigned int m_tileColor;
    std::string getTileType(const std::pair<float, float>&) const;
    std::map<std::pair<int, int>, Chunk> m_active_chunks;
    std::deque<std::pair<int, int>> m_required_chunks;
    std::deque<std::pair<int, int>> m_unrequired_chunks;
    std::array<std::string, 10> tilevariation = {
        "plains", "beach", "dirt", "ocean", "forest", "mountain", "swamp", "jungle", "frozenplain", "snow"
    };
    // Member functions
    void update(float dt);
    void getRequiredChunks();
    void f_drawChunks(sf::RenderWindow& window) const;
    void ChunkBorders(sf::RenderWindow& window) const;
    void initElevationLayer(uint32_t& seed);
};