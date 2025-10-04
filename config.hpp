#pragma once

// Including all necessary libraries
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
#include <optional>
#include <string>
#include <utility>
#include <windows.h>
#include <sstream>

// Debugging macro to log messages
#ifdef _DEBUG
#define LOG(x) { std::ostringstream oss; oss << x; OutputDebugStringA(oss.str().c_str()); }

#else
#define LOG(x) ((void)0)
#endif

// Global variables
namespace esrovar {
    // Compile-time constants
    constexpr unsigned int SCRWDT = 1920u;
    constexpr unsigned int SCRHGT = 1080u;
    constexpr unsigned int FPS = 60u;
    constexpr int CHUNK_SIZE = 16;
    constexpr int CHUNK_RADIUS = 2;
    constexpr int PLAYER_SPRITE = 64;
    // Declaring states and directional constants
    enum class State : std::uint8_t { idle = 0, walk, slash, jump, sit, COUNT };
    enum class Directions : std::uint8_t { up = 0, left, down, right, COUNT };
    inline constexpr std::size_t StateCount = static_cast<std::size_t>(State::COUNT);
    inline constexpr std::size_t DirectionCount = static_cast<std::size_t>(Directions::COUNT);
    constexpr std::size_t to_index(State s) noexcept { return static_cast<std::size_t>(s);}
    constexpr std::size_t to_index(Directions d) noexcept { return static_cast<std::size_t>(d); }
    constexpr std::array<int, StateCount> kFrameCount = { 2, 9, 6, 5, 3};
    constexpr std::array<std::string_view, StateCount> kTexturePaths = {
        "res/player_sprite/idle.png",
        "res/player_sprite/walk.png",
        "res/player_sprite/slash.png",
        "res/player_sprite/jump.png",
        "res/player_sprite/sit.png"};
	extern std::array<sf::Texture, StateCount> kTextures;
    static_assert(kFrameCount.size() == StateCount);
    static_assert(kTexturePaths.size() == StateCount);
    // Declaring global variables
    extern int pixel_size;
    extern int frame_count;
    extern int scale;
    extern int seed;
    extern float player_size;
    extern float timer;
    extern float speed;
    extern int chunkarea;
    extern float movedirx;
    extern float movediry;
    extern float boost;
    extern float totalspeed;
    extern sf::RenderWindow GameWindow;
    extern std::pair<int, int> PLAYER_POSITION;
}// namespace esrovar ends

// Global functions
namespace esrofn {

    bool LoadSpriteSheetsnew();
    int GenerateWorldSeed();
}// namespace esrofn ends

// Global objects and classes
namespace esroops {

    class IUpdatable {
    public:
        virtual void update(float dt) = 0;
        virtual ~IUpdatable() = default;
        IUpdatable() = default;
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
        Chunk(int x, int y);
        ~Chunk() = default;
        int m_chunkX;
        int m_chunkY;
		bool m_isGenerated;
        Tile* getTileData(int x, int y);
        Tile tiles[esrovar::CHUNK_SIZE][esrovar::CHUNK_SIZE];
        void generate(const std::string& tilesheet, sf::Vector2i tilesize);
    private:
        // Member functions
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
        // Member variables
        sf::VertexArray m_grid;
        sf::Texture m_tileset;
    };

    class Player : public sf::Drawable, public sf::Transformable, public sf::Texture, public IUpdatable {
    public:
		// Initializing player object
        Player();
        ~Player() = default;
		
        // Member variables
        bool m_IsMoving;
		esrovar::State m_StateEnum;
		esrovar::State m_PrevStateEnum;
        esrovar::Directions m_DirectionEnum;
        sf::Vector2f m_playerXY;
        int m_TotalFrames;
        int m_CurrentFrame;
        float m_AnimTimer;
        float m_AnimDuration;
        int m_health;
		
        // Member functions
        void update(float dt) override;
        void animatesprite(float dt);
    private:
		
        // Member variables
        sf::Texture m_playerbody;
        std::optional<sf::Sprite> m_playersprite;
		
        // Member functions
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };

    class WorldManager {
        public:
			WorldManager() = default;
            WorldManager(std::pair<int, int>);
            ~WorldManager() = default;
            int m_playerchunk_X;
            int m_playerchunk_Y;
            std::map<std::pair<int, int>, Chunk> m_active_chunks;
            unsigned int m_world_seed;
            void f_drawChunks(sf::RenderWindow& window);
            //void _update_world();
        private:
            void f_initialize_world();
    };
}// namespace esroops ends