#include "config.hpp"

//  Game Main Function
int main() {
    // Load Assets
    esrofn::LoadSpriteSheets();

    // Initializing esrovar::GameWindow and framerate
    sf::Clock clock;
    esroops::Chunk chunk;
    esroops::TileMap tilemap;
    esroops::Player player;
    std::vector <esroops::IUpdatable*> systems;

    // Load world tile
    tilemap.load(esrovar::TileImagePATH, sf::Vector2u(esrovar::pixel_size, esrovar::pixel_size), &chunk.tiles[0][0], esrovar::CHUNK_SIZE, esrovar::CHUNK_SIZE);

    //Init View
    sf::View view;
    sf::Vector2f viewArea = {esrovar::SCRWDT, esrovar::SCRHGT};
    view.setSize(viewArea);

    //Main game loop
    while (esrovar::GameWindow.isOpen()) {
        esrovar::GameWindow.setFramerateLimit(esrovar::FPS);
        
        //Close esrovar::GameWindow on close
        while (const std::optional event = esrovar::GameWindow.pollEvent()) {

            if (event -> is <sf::Event::Closed>())
                esrovar::GameWindow.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
                esrovar::GameWindow.close();
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
            float length = std::sqrt((direction.x * direction.x) + (direction.y * direction.y));
            // Normalize
            direction /= length;
        }

        // Move player
        player.move(direction * esrovar::totalspeed * esrovar::dt);

        // Smoot scrolling view
        sf::Vector2f viewcenter = view.getCenter();
        sf::Vector2f targetcenter = player.getPosition();
        float lerpfactor = 5.0f * esrovar::dt;
        view.setCenter(viewcenter + (targetcenter - viewcenter) * lerpfactor);
        
        // GameWindow initialization
        esrovar::GameWindow.clear();
        esrovar::GameWindow.setView(view);
        esrovar::GameWindow.draw(tilemap);
        esrovar::GameWindow.draw(player);
        esrovar::GameWindow.setView(esrovar::GameWindow.getDefaultView());
        esrovar::GameWindow.display();
    }

    return EXIT_SUCCESS;
}
    