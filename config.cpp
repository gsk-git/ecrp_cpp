#include "config.hpp"
#include <sfml/Graphics.hpp>
#include <FastNoise/FastNoise.h>
#include <windows.h>
#include <iostream>

namespace esrovar {

    unsigned int pixel_size = 50u;
    unsigned int frame_count = 0u;
    unsigned int scale = 2u;
    float player_size = 50.0f;
    float timer = 0.0f;
    float speed = 100.0f;
    int chunkarea = CHUNK_SIZE * pixel_size;
    float movedirx;
    float movediry;
    float boost;
    float dt;
    float totalspeed;

    bool KeyPressed_w = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
    bool KeyPressed_a = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    bool KeyPressed_s = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
    bool KeyPressed_d = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
    bool KeyPressed_up = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
    bool KeyPressed_left = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left);
    bool KeyPressed_down = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down);
    bool KeyPressed_right = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right);
}

namespace esroops {

    // Correct: defining constructor outside class but within the namespace
    Chunk::Chunk() {
        for (int y = 0; y < esrovar::CHUNK_SIZE; ++y) {
            for (int x = 0; x < esrovar::CHUNK_SIZE; ++x) {
                tiles[y][x].type = BlockType::plains;
            }
        }
    }

    Chunk::~Chunk() {
        // Cleanup if needed
    }

    Tile* Chunk::getTileData(int x, int y) {
        return &tiles[x][y];
    }

    bool TileMap::load(const std::string& tilesheet, sf::Vector2u tilesize, const Tile* tile, int width, int height) {
        
        // Loading input tilesheet
        if (!m_tileset.loadFromFile(tilesheet))
            return false;
        
        // Set vertex properties
        m_grid.setPrimitiveType(sf::PrimitiveType::TriangleStrip);
        m_grid.resize(static_cast<size_t>(width) * height * 6);

        // Instantiating vertex grid
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                
                int tileIndex = (x + y * width) * 6;

                int tu = 0;
                int tv = 0;

                float tx = static_cast<float>(x * tilesize.x);
                float ty = static_cast<float>(y * tilesize.y);

                float texX = static_cast<float>(tu * tilesize.x);
                float texY = static_cast<float>(tv * tilesize.y);

                // Triangle 1
                m_grid[static_cast<size_t>(tileIndex) + 0].position = sf::Vector2f(tx, ty);
                m_grid[static_cast<size_t>(tileIndex) + 1].position = sf::Vector2f(tx + tilesize.x, ty);
                m_grid[static_cast<size_t>(tileIndex) + 2].position = sf::Vector2f(tx, ty + tilesize.y);
                m_grid[static_cast<size_t>(tileIndex) + 0].texCoords = sf::Vector2f(texX, texY);
                m_grid[static_cast<size_t>(tileIndex) + 1].texCoords = sf::Vector2f(texX + tilesize.x, texY);
                m_grid[static_cast<size_t>(tileIndex) + 2].texCoords = sf::Vector2f(texX, texY + tilesize.y);
                // Triangle 2
                m_grid[static_cast<size_t>(tileIndex) + 3].position = sf::Vector2f(tx + tilesize.x, ty);
                m_grid[static_cast<size_t>(tileIndex) + 4].position = sf::Vector2f(tx + tilesize.x, ty + tilesize.y);
                m_grid[static_cast<size_t>(tileIndex) + 5].position = sf::Vector2f(tx, ty + tilesize.y);
                m_grid[static_cast<size_t>(tileIndex) + 3].texCoords = sf::Vector2f(texX + tilesize.x, texY);
                m_grid[static_cast<size_t>(tileIndex) + 4].texCoords = sf::Vector2f(texX + tilesize.x, texY + tilesize.y);
                m_grid[static_cast<size_t>(tileIndex) + 5].texCoords = sf::Vector2f(texX, texY + tilesize.y);

            }
        }

        return true;
    }

    void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        states.transform *= getTransform();
        states.texture = &m_tileset;
        target.draw(m_grid, states);
    }

    Player::Player() {
   
    }

}
