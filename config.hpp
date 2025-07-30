#pragma once
#include <sfml/Graphics.hpp>
#include <sfml/Window.hpp>
#include <FastNoise/FastNoise.h>
#include <windows.h>
#include <iostream>
#include <map>

#define LOG(x) { std::ostringstream oss; oss << x; OutputDebugStringA(oss.str().c_str()); }

// Global Variables
namespace esrovar {

    // Compile-time constants
    constexpr unsigned int SCRWDT = 1920u;
    constexpr unsigned int SCRHGT = 1080u;
    constexpr unsigned int FPS = 60u;
    constexpr int CHUNK_SIZE = 16;
    constexpr int PLAYER_SPRITE = 64;

    // Runtime variables
    extern unsigned int pixel_size;
    extern unsigned int frame_count;
    extern unsigned int scale;
    extern float player_size;
    extern float timer;
    extern float speed;
    extern int chunkarea;
    extern float movedirx;
    extern float movediry;
    extern float boost;
    extern float dt;
    extern float totalspeed;
    extern sf::RenderWindow GameWindow;
    extern std::map <std::string, std::string> TexturePath;
    extern std::map <std::string, sf::Texture> TextureFace;
	extern std::string TileImagePATH;
	extern std::string PlayerSpriteImagePATH;
    extern std::string states[5];
    extern std::string FaceDirection[4];

}

// Global objects and classes
namespace esroops {

    class IUpdatable {
        public: 
            virtual void update(float dt) = 0;
            virtual ~IUpdatable() = default;
    };

    enum BlockType {
        plains,
        beach,
        ocean
    };

    enum directions {
        up,
        left,
        down,
        right
    };

    struct Tile {
        BlockType type;
    };

    class Chunk {
        public:
            Chunk();   // Declare constructor
            ~Chunk() = default;  // Declare destructor
            Tile* getTileData(int x, int y);
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
            void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
            // Member variables
            sf::VertexArray m_grid;
            sf::Texture m_tileset;
    };

    class Player : public sf::Drawable, public sf::Transformable, public sf::Texture, public IUpdatable {
        
        public:
            // Member variables
            bool m_IsMoving;
            std::string m_State;
            std::string m_direction;
            sf::Vector2f m_playerXY;
            int m_TotalFrames;
            int m_CurrentFrame;
            float m_AnimTimer;
            float m_AnimDuration;
            int health;

            // Member constructor and functions
            Player();
            ~Player() = default;
            void update(float dt) override;
            void animatesprite(float dt);

        private:
            sf::Texture m_playerbody;
            std::optional<sf::Sprite> m_playersprite;
            void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };
}

namespace esrofn {

    void LoadSpriteSheets();

}
