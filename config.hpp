#pragma once

// Including necessary libraries
#include <sfml/Graphics.hpp>
#include <sfml/Window.hpp>
#include <FastNoise/FastNoise.h>
#include <windows.h>
#include <iostream>
#include <map>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <debugapi.h>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <set>
#include <utility>
#define LOG(x) { std::ostringstream oss; oss << x; OutputDebugStringA(oss.str().c_str()); }

// Global variables
namespace esrovar {

    // Compile-time constants
    constexpr unsigned int SCRWDT = 1920u;
    constexpr unsigned int SCRHGT = 1080u;
    constexpr unsigned int FPS = 60u;
    constexpr int CHUNK_SIZE = 16;
    constexpr int CHUNK_RADIUS = 2;
    constexpr int PLAYER_SPRITE = 64;
    // Runtime variables
    extern unsigned int pixel_size;
    extern unsigned int frame_count;
    extern unsigned int scale;
    extern unsigned int seed;
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
    extern std::pair<int, int> PLAYER_POSITION;
}

// Global functions
namespace esrofn {

    void LoadSpriteSheets();
    int GenerateWorldSeed();
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
        ocean,
        dirt,
        forest,
        jungle,
        swamp,
        mountain,
        frozenplain,
        snow
    };

    struct Tile {
        BlockType type;
    };

    class Chunk : public sf::Drawable, public sf::Transformable {
    public:
        Chunk() = default;
        Chunk(int x, int y);   // Declare constructor
        ~Chunk() = default;  // Declare destructor
        int m_chunkX;
        int m_chunkY;
        Tile* getTileData(int x, int y);
        Tile tiles[esrovar::CHUNK_SIZE][esrovar::CHUNK_SIZE];
        bool generate(const std::string& tilesheet, sf::Vector2u tilesize);
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
        int m_health;
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

    class WorldManager {
        public:
            WorldManager(std::pair<int, int>);
            ~WorldManager() = default;
            int _player_X;
            int _player_Y;
            std::map<std::pair<int, int>, Chunk> _active_chunks;
            unsigned int _world_seed;
            void _drawChunks(sf::RenderWindow& window);
        private:
            void _initialize_world();
            //void _update_world();
    };
}
