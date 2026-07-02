#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include <string>
#include <utility>
#include <array>

// Compile-time constants
constexpr int PLAYER_SPRITE = 64;
enum class State : std::uint8_t { idle = 0, walk, slash, jump, sit, run, COUNT };
enum class Directions : std::uint8_t { up = 0, left, down, right, COUNT };
inline constexpr std::size_t StateCount = static_cast<std::size_t>(State::COUNT);
inline constexpr std::size_t DirectionCount = static_cast<std::size_t>(Directions::COUNT);
constexpr std::size_t to_index(State s) noexcept { return static_cast<std::size_t>(s); }
constexpr std::size_t to_index(Directions d) noexcept { return static_cast<std::size_t>(d); }
constexpr std::array<int, StateCount> kFrameCount = { 2, 9, 6, 5, 3, 8 };
constexpr std::array<std::string_view, StateCount> kStateNames = { "idle", "walk", "slash", "jump", "sit", "run" };
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
extern sf::Texture tileset;
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
extern std::pair<float, float> PLAYER_POSITION;

// Player functions
inline std::tuple<int, int> getPlayerXY(std::pair<float, float> playerxy) {
	// Getting floored player coordinates
	float x = std::floor(playerxy.first);
	float y = std::floor(playerxy.second);

	return std::make_tuple(static_cast<int>(x), static_cast<int>(y));
}

// Player & Utility Classes
class Player : public sf::Drawable, public sf::Transformable, public sf::Texture {
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
    State m_StateEnum;
    State m_PrevStateEnum;
    Directions m_DirectionEnum;
    std::optional<sf::Sprite> m_playersprite;
    // Member functions
    void update(float dt);
    void animatesprite(float dt);
    void setOrigintoBottomCenter();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
struct PlayerData {
    float m_playerposX;
    float m_playerposY;
};