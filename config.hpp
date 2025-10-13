#pragma once

// Including all necessary libraries
#include <map>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include <string>
#include <utility>
#include <windows.h>
#include <sstream>
#include "FastNoise2/include/FastNoise/FastNoiseLite.h"

// Debugging macro to log messages
#ifdef _DEBUG
#define LOG(x) do { std::ostringstream _oss; \
    _oss << x; \
    std::string _s = _oss.str(); \
    _s.push_back('\n'); \
    OutputDebugStringA(_s.c_str()); \
} while(0)
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
    enum class State : std::uint8_t { idle = 0, walk, slash, jump, sit, run, COUNT };
    enum class Directions : std::uint8_t { up = 0, left, down, right, COUNT };
    inline constexpr std::size_t StateCount = static_cast<std::size_t>(State::COUNT);
    inline constexpr std::size_t DirectionCount = static_cast<std::size_t>(Directions::COUNT);
    constexpr std::size_t to_index(State s) noexcept { return static_cast<std::size_t>(s);}
    constexpr std::size_t to_index(Directions d) noexcept { return static_cast<std::size_t>(d); }
    constexpr std::array<int, StateCount> kFrameCount = { 2, 9, 6, 5, 3, 8};
    constexpr std::array<std::string_view, StateCount> kStateNames = { "idle", "walk", "slash", "jump", "sit", "run"};
    constexpr std::array<std::string_view, StateCount> kTexturePaths = {
        "res/player_sprite/idle.png",
        "res/player_sprite/walk.png",
        "res/player_sprite/slash.png",
        "res/player_sprite/jump.png",
        "res/player_sprite/sit.png",
        "res/player_sprite/run.png" };
	extern std::array<sf::Texture, StateCount> kTextures;
    static_assert(kFrameCount.size() == StateCount);
    static_assert(kTexturePaths.size() == StateCount);
	extern sf::Font mainfont;
    extern sf::Texture tileset;
    extern bool ChunkBorder;
    extern bool DebugMode;
    // Declaring global variables
    extern int pixel_size;
    extern int frame_count;
    extern int scale;
    extern float player_size;
    extern float timer;
    extern float speed;
    extern int chunkarea;
    extern float movedirx;
    extern float movediry;
    extern float boost;
    extern float totalspeed;
    extern int jumpboost;
    extern sf::RenderWindow GameWindow;
	extern FastNoiseLite noise;
    extern std::pair<float, float> PLAYER_POSITION;
}// namespace esrovar ends

// Global functions
namespace esrofn {

    bool LoadSpriteSheetsnew();
    bool LoadTileSheet();
    bool LoadFonts();
    [[nodiscard]] inline uint32_t  GenerateWorldSeed() noexcept;
    std::tuple<int, int> getChunkXY(std::pair<float, float>);
    std::tuple<int, int> getPlayerXY(std::pair<float, float>);
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
        void generate(sf::Vector2i tilesize, int ChunkColor);
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
		bool m_IsSlashAttacking;
		bool m_IsJumping;
		bool m_IsSitting;
		bool m_IsRunning;
		esrovar::State m_StateEnum;
		esrovar::State m_PrevStateEnum;
        esrovar::Directions m_DirectionEnum;
        sf::Vector2f m_playerXY;
        int m_TotalFrames;
        int m_CurrentFrame;
        float m_AnimTimer;
        float m_AnimDuration;
        int m_health;
		void setOrigintoBottomCenter();
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
            WorldManager(std::pair<int, int>, int seed);
            ~WorldManager() = default;
            int m_playerchunk_X;
            int m_playerchunk_Y;
            std::map<std::pair<int, int>, Chunk> m_active_chunks;
            unsigned int m_world_seed;
            void f_drawChunks(sf::RenderWindow& window);
            void update(int seed);
			void ChunkBorders(sf::RenderWindow& window);
        private:
            void f_initialize_world();
    };

    class HudBox : public sf::Drawable, public sf::Transformable {
        public:
            using sf::Transformable::setPosition;
            HudBox(sf::Vector2f size, sf::Vector2f position, sf::Color color, sf::Color outcolor, float thickness) {
				hudbox.setSize(size);
				hudbox.setFillColor(color);
				hudbox.setOutlineColor(outcolor);
				hudbox.setOutlineThickness(thickness);
                setPosition(position);
            }
			HudBox() = default;
			~HudBox() override = default;
            using sf::Transformable::getPosition;
            void setSize(sf::Vector2f size)             { hudbox.setSize(size);}
            void setFillColor(sf::Color& color)         { hudbox.setFillColor(color); }
            void setOutlineColor(sf::Color& outcolor)   { hudbox.setOutlineColor(outcolor); }
            void setOutlineThickness(float thickness)  { hudbox.setOutlineThickness(thickness); }
        private:
            sf::RectangleShape hudbox;
            void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
                states.transform *= getTransform();
                target.draw(hudbox, states);
            }
    };

    class HudText : public sf::Drawable, public sf::Transformable {

    };

}// namespace esroops ends