#include "config.hpp"

//  Game Main Function
int main() {

    // Initializing window and framerate
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "ESRO", sf::Style::None);
    sf::Clock clock;
    esroops::Chunk chunk;
    esroops::TileMap tilemap;
    std::vector <esroops::IUpdatable*> systems;

    // Load world tile
    tilemap.load("res/tile.png",sf::Vector2u(esrovar::pixel_size, esrovar::pixel_size), &chunk.tiles[0][0], esrovar::CHUNK_SIZE, esrovar::CHUNK_SIZE);
    //tilemap.setPosition(sf::Vector2f(static_cast<float>(window.getSize().x) * 0.5f, static_cast<float>(window.getSize().y) * 0.5f));
    //tilemap.setOrigin({ 400.0, 400.0 });
    
    //Player
    sf::RectangleShape playerbody;
    playerbody.setSize(sf::Vector2f({ esrovar::player_size, esrovar::player_size }));
    playerbody.setFillColor(sf::Color::Cyan);
    playerbody.setPosition(sf::Vector2f(static_cast<float>(window.getPosition().x * 0.5f), static_cast<float>(window.getPosition().y * 0.5f)));
    playerbody.setOrigin({(playerbody.getSize().x * 0.5f), (playerbody.getSize().y * 0.5f)});

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

        esrovar::movedirx = 0.f;
        esrovar::movediry = 0.f;
        esrovar::boost = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ? 2.0f : 1.0f;
        esrovar::dt = clock.restart().asSeconds();
        esrovar::totalspeed = esrovar::speed * esrovar::boost;

        for (auto system : systems) {
            system->update(esrovar::dt);
        }

        //LOG("Hello from debug string!" << totalspeed << "\n");

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
            esrovar::movediry = -esrovar::totalspeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {         
            esrovar::movediry = esrovar::totalspeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
            esrovar::movedirx = -esrovar::totalspeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
            esrovar::movedirx = esrovar::totalspeed;
        }

        // Get directions vector
        sf::Vector2f direction({ esrovar::movedirx, esrovar::movediry });

        // Check if directions are not 0
        if (direction.x != 0.f || direction.y != 0.f) {
            // Getting hypotenuse length
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            // Normalize
            direction /= length;
        }

        // Move player
        playerbody.move(direction * esrovar::totalspeed * esrovar::dt);

        // Smoot scrolling view
        sf::Vector2f viewcenter = view.getCenter();
        sf::Vector2f targetcenter = playerbody.getPosition();
        float lerpfactor = 5.0f * esrovar::dt;
        view.setCenter(viewcenter + (targetcenter - viewcenter) * lerpfactor);
        
        // Window initialization
        window.clear();
        window.setView(view);
        window.draw(tilemap);
        window.draw(playerbody);
        window.setView(window.getDefaultView());
        window.display();
    }

    return EXIT_SUCCESS;
}
    