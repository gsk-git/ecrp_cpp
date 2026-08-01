#pragma once

// Including all necessary libraries
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Transformable.hpp>

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