#include "config.hpp"
#include <sfml/Graphics.hpp>
#include <sfml/Window.hpp>
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
    int Chunkrea = CHUNK_SIZE * pixel_size;
    float movedirx;
    float movediry;
    float boost;
    float dt;
    float totalspeed;
    std::string TileImagePATH = "res/tile.png";
    std::string PlayerSpriteImagePATH = "res/player_sprite/";
    std::string states[5] = { "idle","walking","attack","jump","sit" };
    std::string FaceDirection[4] = { "up","left","down","right"};
    sf::VideoMode desktop = desktop.getDesktopMode();
    sf::RenderWindow GameWindow(desktop, "ESRO", sf::Style::None);
    std::map <std::string, sf::Texture> TextureFace;
    std::map<std::string, std::string> TexturePath = {
        {"idle", PlayerSpriteImagePATH+"idle.png"},
        {"walk", PlayerSpriteImagePATH + "walk.png"},
        {"jump", PlayerSpriteImagePATH + "jump.png"},
        {"sit", PlayerSpriteImagePATH + "sit.png"},
        {"attack", PlayerSpriteImagePATH + "slash.png"}
    };
    std::map<std::string, sf::IntRect> TextureRects = {
      {"up",    sf::IntRect({0, 0 * 64}, {64, 64})},
      {"left",  sf::IntRect({0, 1 * 64}, {64, 64})},
      {"down",  sf::IntRect({0, 2 * 64}, {64, 64})},
      {"right", sf::IntRect({0, 3 * 64}, {64, 64})}
    };


}

namespace esrofn {

    void LoadSpriteSheets() {
        try {

			for (auto& [name, path] : esrovar::TexturePath) {

				sf::Texture img;
				if (!img.loadFromFile(path))
					LOG("Image Path or File not found")
				esrovar::TextureFace.emplace(name, std::move(img));

			}
        }

        catch (const std::exception& e) {

            std::cerr << "Error: " << e.what() << "\n";
        }
        
    }

}

namespace esroops {

    Chunk::Chunk() {

        for (int y = 0; y < esrovar::CHUNK_SIZE; ++y)
            for (int x = 0; x < esrovar::CHUNK_SIZE; ++x) {
                tiles[x][y].type = BlockType::plains;
			}
        Tile* getTileData(int x, int y);
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
                
                // tileIndex identifies where the tile will sit in this grid
                int tileIndex = (x + y * width) * 6;

                // Tile sheet index
                int tu = 0;
                int tv = 0;

                // XY of independent tile
                float tx = static_cast<float>(x * tilesize.x);
                float ty = static_cast<float>(y * tilesize.y);

                // XY for independent tile texture
                float texX = static_cast<float>(tu * tilesize.x);
                float texY = static_cast<float>(tv * tilesize.y);

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

            }
        }

        return true;
    }

    void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {

        states.transform *= getTransform();
        states.texture = &m_tileset;
        target.draw(m_grid, states);
    }

    Player::Player(){

        // Default init
        m_IsMoving = false;
        m_State = esrovar::states[0];
        m_direction = esrovar::FaceDirection[2];
        m_playerXY = sf::Vector2f(static_cast<float>(esrovar::GameWindow.getPosition().x * 0.5f), static_cast<float>(esrovar::GameWindow.getPosition().y * 0.5f));

        // Default sprite and position
        m_playersprite.emplace(esrovar::TextureFace[m_State]);
        m_playersprite->setTextureRect(esrovar::TextureRects[m_direction]);
        m_playersprite->setPosition(m_playerXY);
    }

    Player::~Player() = default;

    void Player::update() {

        // Flag check for player movement
        m_IsMoving = (m_playerXY.x != 0 && m_playerXY.y != 0);

        if (m_IsMoving) 
            m_State = esrovar::states[0];
        else
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

}
