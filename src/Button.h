#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
    
    sf::Color idleColor;
    sf::Color hoverColor;
    sf::Color activeColor;
    sf::Color disabledColor;
    
    sf::Color textIdleColor;
    sf::Color textDisabledColor;

    bool isHovered;
    bool isEnabled;

public:
    Button(float x, float y, float width, float height,
           const std::string& buttonText, sf::Font& font, unsigned int characterSize = 18);

    void update(const sf::Vector2f& mousePos);
    void draw(sf::RenderWindow& window);
    bool isPressed(const sf::Vector2f& mousePos, bool mouseClicked);

    void setEnabled(bool enabled);
    bool getEnabled() const { return isEnabled; }
    
    void setPosition(float x, float y);
    void setText(const std::string& newText);
};

class ChipButton {
private:
    sf::Sprite sprite;
    sf::Text text;
    bool isHovered;
    bool isEnabled;
    int chipValue;
public:
    ChipButton(float x, float y, int value, const std::string& label, sf::Font& font, sf::Texture& texture, sf::IntRect textureRect);
    void update(const sf::Vector2f& mousePos);
    void draw(sf::RenderWindow& window);
    bool isPressed(const sf::Vector2f& mousePos, bool mouseClicked);
    void setTexture(const sf::Texture& texture);
    void setEnabled(bool enabled);
    bool getEnabled() const { return isEnabled; }
    int getValue() const { return chipValue; }
};
