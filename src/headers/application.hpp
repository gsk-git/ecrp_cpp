#pragma once
#include <SFML/Graphics.hpp>
#include "stateManager.hpp"
#include <optional>
#include <memory>

class application {
private:
    // Member variables
    sf::RenderWindow m_window;
    sf::Clock m_clock;
    stateManager m_statemanager;

    // Member functions
    void processEvents() {
        while (const std::optional<sf::Event> event = m_window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                m_window.close();
                return;
            }

            m_statemanager.handleEvent(*event);
        }
    };
public:
    application() : m_window(sf::VideoMode::getDesktopMode(), "ESRO™ v0.1.0", sf::Style::None), m_statemanager(m_window) {};
    void run() {
        while (m_window.isOpen())
        {
            processEvents();

            if (!m_window.isOpen()) break;

            float dt = m_clock.restart().asSeconds();

            m_statemanager.update(dt);

            if (m_statemanager.processPendingRequest()) m_window.close();

            if (m_window.isOpen()) {
                m_window.clear();
                m_statemanager.render(m_window);
                m_window.display();
            }
        }
        m_window.close();
    };
};