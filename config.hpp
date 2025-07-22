#pragma once
#include <sfml/Graphics.hpp>

namespace esrovar {
    // Compile-time constants
    constexpr unsigned int SCRWDT = 1920u;
    constexpr unsigned int SCRHGT = 1080u;
    constexpr unsigned int FPS = 60u;
    constexpr int CHUNK_SIZE = 16;

    // Runtime variables (declared with extern, defined in .cpp)
    extern unsigned int pixel_size;
    extern unsigned int frame_count;
    extern unsigned int scale;
    extern float player_size;
    extern float timer;
    extern float speed;
    extern int chunkarea;
}

namespace esroops {
    enum BlockType {
        plains,
        beach,
        ocean,
    };

    struct Tile {
        BlockType type;
    };

    class Chunk {
    public:
        Chunk();   // Declare constructor
        ~Chunk();  // Declare destructor
        Tile* getTileData();
        Tile tiles[esrovar::CHUNK_SIZE][esrovar::CHUNK_SIZE];
    };

    class TileMap: public sf::Drawable, public sf::Transformable {
    // Objective
    // tile.load("res/tile.png", sf::Vectoru(50,50), &chunk.tiles[0][0], CHUNK_SIZE, CHUNK_SIZE);
        public:
            // Member functions
            bool load(const std::string& tilesheet, sf::Vector2u tilesize, const Tile* tile, int x, int y);
        private:
            // Member functions
            virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
            // Member variables
            sf::VertexArray m_grid;
            sf::Texture m_tileset;
    };
}
