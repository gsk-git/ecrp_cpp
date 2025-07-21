#include <FastNoise/FastNoise.h>
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <iostream>
#include "config.hpp"

#define LOG(x) { std::ostringstream oss; oss << x; OutputDebugStringA(oss.str().c_str()); }

//  Game Main Function
int main() {

    // Initializing window and framerate
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "ESRO", sf::Style::Close | sf::Style::Resize | sf::Style::Default);
    sf::Clock clock;
    esroops::Chunk chunk;
    esroops::TileMap tilemap;
    
    // Load world tile
    tilemap.load("res/tile.png",sf::Vector2u(esrovar::pixel_size, esrovar::pixel_size), &chunk.tiles[0][0], esrovar::CHUNK_SIZE, esrovar::CHUNK_SIZE);
    tilemap.setPosition(sf::Vector2f(static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) / 2.f));
    tilemap.setOrigin({ 400.0, 400.0 });
    
    //Player
    sf::RectangleShape playerbody;
    playerbody.setSize(sf::Vector2f({ esrovar::player_size, esrovar::player_size }));
    playerbody.setFillColor(sf::Color::Cyan);
    playerbody.setPosition(sf::Vector2f(static_cast<float>(window.getSize().x) / 2.f, static_cast<float>(window.getSize().y) / 2.f));
    playerbody.setOrigin({(playerbody.getSize().x / 2), (playerbody.getSize().y / 2)});
    //Init View
    sf::View view;
    sf::Vector2f viewArea = {esrovar::SCRWDT, esrovar::SCRHGT};
    view.setSize(viewArea);

    //Main game loop
    while (window.isOpen()) {
        window.setFramerateLimit(esrovar::FPS);
        
        //Close window on close
        while (const std::optional event = window.pollEvent()) {
            if (event -> is <sf::Event::Closed>())
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                window.close();
            }
        }

        float movedirx = 0.f;
        float movediry = 0.f;
        float boost = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ? 2.0f : 1.0f;
        float deltatime = clock.restart().asSeconds();
        float totalspeed = esrovar::speed * boost;

        //LOG("Hello from debug string!" << totalspeed << "\n");


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            movediry = -totalspeed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {         
            movediry = totalspeed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            movedirx = -totalspeed;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            movedirx = totalspeed;
        }

        // Get directions vector
        sf::Vector2f direction(movedirx, movediry);
        // Check if directions are not 0
        if (direction.x != 0.f || direction.y != 0.f) {
            // Getting hypotenuse length
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            // Normalize
            direction /= length;
        }

        // Move player
        playerbody.move(direction * totalspeed * deltatime);
        
        // Window initialization
        window.clear();
        window.draw(tilemap);
        window.draw(playerbody);
        window.display();
     }

    return EXIT_SUCCESS;
}
    