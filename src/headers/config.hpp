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
    constexpr int PLAYER_SPRITE = 64;
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
	extern std::string playerFileURI;
    extern std::string saveFile;
	extern std::array<sf::Texture, StateCount> kTextures;
	extern sf::Font mainfont;
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
    extern std::pair<float, float> PLAYER_POSITION;
}// namespace esrovar ends

// Global functions
namespace esrofn {

    [[maybe_unused]] static void initTempHumLayer();

    [[maybe_unused]] static void initBiomeLayer();

    [[maybe_unused]] static void initRiverLayer();
}

// Global objects and classes
namespace esroops {

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
            void f_drawChunks (sf::RenderWindow& window) const;
            void ChunkBorders(sf::RenderWindow& window) const;
            void f_initialize_world();
			void initElevationLayer(uint32_t& seed);
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
            unsigned int size = 18) : textbox(font, "", size) {
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