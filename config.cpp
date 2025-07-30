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
    int chunkrea = CHUNK_SIZE * pixel_size;
    float movedirx;
    float movediry;
    float boost;
    float dt;
    float totalspeed;
    const char format[5] = ".png";
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
        {states[2], PlayerSpriteImagePATH + states[2] + format}
    };
    std::map<std::string, sf::IntRect> TextureRects = {
      {"up",    sf::IntRect({0 * PLAYER_SPRITE, 0 * PLAYER_SPRITE}, {PLAYER_SPRITE, PLAYER_SPRITE})},
      {"left",  sf::IntRect({0 * PLAYER_SPRITE, 1 * PLAYER_SPRITE}, {PLAYER_SPRITE, PLAYER_SPRITE})},
      {"down",  sf::IntRect({0 * PLAYER_SPRITE, 2 * PLAYER_SPRITE}, {PLAYER_SPRITE, PLAYER_SPRITE})},
      {"right", sf::IntRect({0 * PLAYER_SPRITE, 3 * PLAYER_SPRITE}, {PLAYER_SPRITE, PLAYER_SPRITE})}
    };
}// namespace esrovar ends

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

}// namespace esrofn ends

namespace esroops {

    Chunk::Chunk() {

        for (int y = 0; y < esrovar::CHUNK_SIZE; ++y)
            for (int x = 0; x < esrovar::CHUNK_SIZE; ++x) {
                tiles[x][y].type = BlockType::plains;
			}
        Tile* getTileData(int x, int y);
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

        // Initializing member variables
        m_IsMoving = false;
        m_State = esrovar::states[0];
        m_direction = esrovar::FaceDirection[2];
        m_playerXY = sf::Vector2f(static_cast<float>(esrovar::GameWindow.getPosition().x * 0.5f), static_cast<float>(esrovar::GameWindow.getPosition().y * 0.5f));
        m_TotalFrames = 0;
        m_CurrentFrame = 0;
        m_AnimTimer = 0.0f;
        m_AnimDuration = 0.0f;

        // Default sprite and position
        m_playersprite.emplace(esrovar::TextureFace[m_State]);
        m_playersprite->setTextureRect(esrovar::TextureRects[m_direction]);
        m_playersprite->setPosition(m_playerXY);
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
}// namespace esroops ends