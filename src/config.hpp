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
#include "FastNoise/include/FastNoise/FastNoiseLite.h"
#include <deque>
#include <SFML/Graphics/Rect.hpp>
#include <vector>

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

// Global variables
namespace esrovar {    
        
    // Compile-time constants
    constexpr unsigned int SCRWDT = 1920u;
    constexpr unsigned int SCRHGT = 1080u;
    constexpr unsigned int FPS = 60u;
    constexpr int CHUNK_SIZE = 16;
    constexpr int CHUNK_RADIUS = 2;
    constexpr int PLAYER_SPRITE = 64;
    constexpr uint32_t gameseed = 23091995;
    constexpr unsigned int pixel_size = 50u; 
    constexpr enum class State : std::uint8_t { idle = 0, walk, slash, jump, sit, run, COUNT };
    constexpr enum class Directions : std::uint8_t { up = 0, left, down, right, COUNT };
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
	constexpr char saveFileURI[23] = "res/user_dat/data.json";
	extern std::array<sf::Texture, StateCount> kTextures;
    extern int chunk_area;
	extern sf::Font mainfont;
    extern sf::Texture tileset;
    extern bool ChunkBorder;
    extern bool DebugMode;
    extern bool Save;   
    extern int frame_count;
    extern int scale;
    extern float player_size;
    extern float timer;
    extern float speed;  
    extern float movedirx;
    extern float movediry;
    extern float boost;
    extern float totalspeed;
    extern int jumpboost;
    extern float globaldelta;
    extern sf::RenderWindow GameWindow;
	extern FastNoiseLite noise;
    extern std::pair<float, float> PLAYER_POSITION;
}// namespace esrovar ends

// Global functions
namespace esrofn {

    bool LoadSpriteSheetsnew();
    
    bool LoadTileSheet();
    
    bool LoadFonts();
    
    std::tuple<int, int> getChunkXY(std::pair<float, float>);
    
    std::tuple<int, int> getPlayerXY(std::pair<float, float>);
    
    std::tuple<int, int> getPlayerChunkXY(std::pair<float, float>);
}

// Global objects and classes
namespace esroops {

    class IUpdatable {
    public:
        virtual void update(float dt) = 0;
        virtual ~IUpdatable() = default;
        IUpdatable() = default;
    };

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
        Tile* getTileData(int x, int y);
        std::vector<Tile> tiles;
        Tile& tileAt(int x, int y);
        const Tile& tileAt(int x, int y) const;
        sf::VertexArray m_grid;
        sf::Texture m_tileset;
        // Member functions
        void generate(sf::Vector2f tilesize);
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };

    class Player : public sf::Drawable, public sf::Transformable, public sf::Texture, public IUpdatable {
    public:
		
        // Initializing player object
        Player();
        ~Player() final = default;		
            
        // Member variables
        bool m_IsMoving;
		bool m_IsSlashAttacking;
		bool m_IsJumping;
		bool m_IsSitting;
		bool m_IsRunning;
        bool m_IsTileOcean;		
        int m_health;
        int m_TotalFrames;
        int m_CurrentFrame;
        float m_AnimTimer;
        float m_AnimDuration;
        sf::Vector2f m_playerXY;
        sf::Texture m_playerbody;
        esrovar::State m_StateEnum;
        esrovar::State m_PrevStateEnum;
        esrovar::Directions m_DirectionEnum;
        std::optional<sf::Sprite> m_playersprite;
            
        // Member functions
        void update(float dt) override;
        void animatesprite(float dt);
        void setOrigintoBottomCenter();
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };

    struct PlayerData {
        float m_playerposX;
        float m_playerposY;
    };

    class WorldManager : public IUpdatable{
        public:
			// Member variables
			WorldManager() = default;
            WorldManager(std::pair<float, float>, int seed);
            ~WorldManager() = default;
            float m_playerchunk_X;
            float m_playerchunk_Y;
            int m_chunkGenerationLimit;
            unsigned int m_world_seed;
			unsigned int m_tileColor;
            std::string getTileType(std::pair<float, float>);
            std::map<std::pair<int, int>, Chunk> m_active_chunks;
            std::deque<std::pair<int, int>> m_required_chunks;
            std::deque<std::pair<int, int>> m_unrequired_chunks;
            std::array<std::string, 10> tilevariation = {
                "plains", "beach", "dirt", "ocean", "forest", "mountain", "swamp", "jungle", "frozenplain", "snow"
			};
			// Member functions
            void update(float dt);
			void getRequiredChunks();
            void f_drawChunks (sf::RenderWindow& window) const;
            void ChunkBorders(sf::RenderWindow& window) const;
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
            void setSize(sf::Vector2f size) { hudbox.setSize(size);}
            void setFillColor(sf::Color color) { hudbox.setFillColor(color); }
            void setOutlineColor(sf::Color outcolor) { hudbox.setOutlineColor(outcolor); }
            void setOutlineThickness(float thickness) { hudbox.setOutlineThickness(thickness); }
        private:
            sf::RectangleShape hudbox;
            void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
                states.transform *= getTransform();
                target.draw(hudbox, states);
            }
    };

    class HudText : public sf::Drawable, public sf::Transformable {
    public:
        explicit HudText(const sf::Font& font,
            sf::Vector2f position = { 0.f, 0.f },
            sf::Color fillColor = sf::Color::White,
            unsigned int style = sf::Text::Regular,
            unsigned int size = 18)
            : textbox(font, "", size)
        {
            textbox.setPosition(position);
            textbox.setFillColor(fillColor);
            textbox.setStyle(style);
        }
        void setCharacterSize(unsigned int size) { textbox.setCharacterSize(size); }
        void setFillColor(const sf::Color& fillColor) { textbox.setFillColor(fillColor); }
        void setStyle(unsigned int style) { textbox.setStyle(style);}
        void setString(const std::string& text) { textbox.setString(text); }
        void setPosition(const sf::Vector2f& position) { textbox.setPosition(position); }
        void setFont(const sf::Font& font) { textbox.setFont(font); }
        sf::FloatRect getLocalBounds() const { return localbounds; }

    private:
        sf::Text textbox;
		sf::FloatRect localbounds;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
            states.transform *= getTransform();
            target.draw(textbox, states);
        }
    };
}