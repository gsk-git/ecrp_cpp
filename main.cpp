// Including necessary libraries
#include "config.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/WindowEnums.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Graphics/View.hpp>
#include <chrono>
#include <Windows.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <optional>
#include <string>

// Handles input
static void InputHandler(esroops::Player& player) {
    esrovar::movedirx = 0.f;
    esrovar::movediry = 0.f;
    esrovar::boost = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ? 2.0f : 1.0f;
    esrovar::totalspeed = esrovar::speed * esrovar::boost;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        esrovar::movediry = -esrovar::totalspeed;
        player.m_direction = esrovar::FaceDirection[0];
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        esrovar::movedirx = -esrovar::totalspeed;
        player.m_direction = esrovar::FaceDirection[1];
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        esrovar::movediry = esrovar::totalspeed;
        player.m_direction = esrovar::FaceDirection[2];
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        esrovar::movedirx = esrovar::totalspeed;
        player.m_direction = esrovar::FaceDirection[3];
    }
}

// Processes window events
static void ProcessWindowEvents() {
    // Process window events
    while (const std::optional event = esrovar::GameWindow.pollEvent()) {
        if (event -> is <sf::Event::Closed>())
            esrovar::GameWindow.close();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            esrovar::GameWindow.close();
        }
    }
}

// Make SFML window transparent
static void makeWindowTransparent(sf::RenderWindow& window) {
    HWND hwnd = static_cast<HWND>(window.getNativeHandle());
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
}

// Runs the splash screen
static void runSplash() {
    sf::RenderWindow splash(sf::VideoMode({ 512, 512 }), "Splash", sf::Style::None);
    makeWindowTransparent(splash);

    sf::Texture logoTexture;
    if (!logoTexture.loadFromFile("res/splash.png"))
		LOG("Logo not found or path is incorrect");
    sf::Sprite logoSprite(logoTexture);

    logoSprite.setScale(sf::Vector2f(static_cast<float>(512.f) / logoTexture.getSize().x, static_cast<float>(512.f) / logoTexture.getSize().y));
    auto bounds = logoSprite.getLocalBounds();
    logoSprite.setOrigin(sf::Vector2f(bounds.size.x * 0.5f, bounds.size.y * 0.5f));
    logoSprite.setPosition(sf::Vector2f(splash.getSize().x * 0.5f, splash.getSize().y * 0.5f));

    auto start = std::chrono::steady_clock::now();
    while (splash.isOpen()) {
        while (const std::optional event = esrovar::GameWindow.pollEvent()) {
            if (event->is <sf::Event::Closed>())
                splash.close();
        }

        // Check if 10 seconds passed
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() >= 10) {
            splash.close();
        }

        splash.clear(sf::Color::Black);
        splash.draw(logoSprite);
        splash.display();
    }
}

// Starts the game
static void StartGame() {
    // Load Assets
    esrofn::LoadSpriteSheets();
    // Initializing esrovar::GameWindow and framerate
    sf::Clock clock;
    esroops::WorldManager world(esrovar::PLAYER_POSITION);
    esroops::Player player;
    std::vector <esroops::IUpdatable*> systemdelta;
    systemdelta.push_back(&player);
    //Init View
    sf::View view;
    sf::Vector2f viewArea = { esrovar::SCRWDT, esrovar::SCRHGT };
    view.setSize(viewArea);
        
    //Main game loop
    while (esrovar::GameWindow.isOpen()) {
        esrovar::dt = clock.restart().asSeconds();
        esrovar::GameWindow.setFramerateLimit(esrovar::FPS);
        //Close esrovar::GameWindow on close
        ProcessWindowEvents();
        // Spike for maintain system wide delta time management
        for (auto system : systemdelta) {
            system->update(esrovar::dt);
        }
        // Handle inputs
        InputHandler(player);
        // Get directions vector
        sf::Vector2f direction({ esrovar::movedirx, esrovar::movediry });
        // Check if directions are not 0
        if (direction.x != 0.f || direction.y != 0.f) {
            // Getting hypotenuse length
            float length = std::sqrt((direction.x * direction.x) + (direction.y * direction.y));
            // Normalize
            direction /= length;
        }
        // Move and update player
        player.move(direction * esrovar::totalspeed * esrovar::dt);
        player.update(esrovar::dt);
        // Smooth scroll view
        sf::Vector2f viewcenter = view.getCenter();
        sf::Vector2f targetcenter = player.getPosition();
        float lerpfactor = 5.0f * esrovar::dt;
        view.setCenter(viewcenter + (targetcenter - viewcenter) * lerpfactor);
        // GameWindow initialization
        esrovar::GameWindow.clear();
        esrovar::GameWindow.setView(view);
        world.f_drawChunks(esrovar::GameWindow);
        esrovar::GameWindow.draw(player);
        esrovar::GameWindow.setView(esrovar::GameWindow.getDefaultView());
        esrovar::GameWindow.display();
    }
}

//  Game Main Function
int main() {
    // Displays Default Splash Image
    runSplash();
    //Starts Game
    StartGame();
    return EXIT_SUCCESS;
}