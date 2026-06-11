#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "GameObject.h"


class Button : public GameObject {
protected:
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
    bool isVisible = true;

    GameState drawState;
    bool hasDrawState = false;

public:
    Button(float x, float y, float width, float height,
           const std::string& buttonText, sf::Font& font, unsigned int characterSize = 18);

    void setDrawState(GameState state);

    void draw(sf::RenderWindow& window) override;
    void updateMouse(const sf::Vector2f& mousePos) override;
    bool shouldDraw(GameState state) const override;

    bool isPressed(const sf::Vector2f& mousePos, bool mouseClicked);

    void setEnabled(bool enabled);
    bool getEnabled() const { return isEnabled; }

    void setVisible(bool visible);
    bool getVisible() const { return isVisible; }
    
    void setPosition(float x, float y);
    void setText(const std::string& newText);
    std::string getTextString() const { return text.getString().toAnsiString(); }
};

class ChipButton : public Button {
private:
    sf::Sprite sprite;
    int chipValue;
public:
    ChipButton(float x, float y, int value, const std::string& label, sf::Font& font, sf::Texture& texture, sf::IntRect textureRect);
    

    void draw(sf::RenderWindow& window) override;
    void updateMouse(const sf::Vector2f& mousePos) override;
    bool isPressed(const sf::Vector2f& mousePos, bool mouseClicked);
    
    void setTexture(const sf::Texture& texture);
    int getValue() const { return chipValue; }
};
