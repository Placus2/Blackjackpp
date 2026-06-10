#include "Button.h"

Button::Button(float x, float y, float width, float height,
               const std::string& buttonText, sf::Font& font, unsigned int characterSize) {
    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(width, height));

    idleColor = sf::Color(20, 35, 25, 220);
    hoverColor = sf::Color(35, 75, 50, 230);
    activeColor = sf::Color(50, 115, 75, 255);
    disabledColor = sf::Color(40, 40, 40, 120);

    shape.setFillColor(idleColor);
    shape.setOutlineThickness(2.f);
    shape.setOutlineColor(sf::Color(76, 175, 80, 200));

    text.setFont(font);
    text.setString(buttonText);
    text.setCharacterSize(characterSize);
    
    textIdleColor = sf::Color::White;
    textDisabledColor = sf::Color(120, 120, 120, 180);
    text.setFillColor(textIdleColor);

    // Center text origin
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
    text.setPosition(x + width / 2.f, y + height / 2.f);

    isHovered = false;
    isEnabled = true;
}

void Button::update(const sf::Vector2f& mousePos) {
    if (!isEnabled) {
        shape.setFillColor(disabledColor);
        shape.setOutlineColor(sf::Color(60, 60, 60, 120));
        text.setFillColor(textDisabledColor);
        return;
    }

    if (shape.getGlobalBounds().contains(mousePos)) {
        isHovered = true;
        shape.setFillColor(hoverColor);
        shape.setOutlineColor(sf::Color(120, 230, 150, 255));
        text.setFillColor(sf::Color::White);
    } else {
        isHovered = false;
        shape.setFillColor(idleColor);
        shape.setOutlineColor(sf::Color(76, 175, 80, 200));
        text.setFillColor(textIdleColor);
    }
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

bool Button::isPressed(const sf::Vector2f& mousePos, bool mouseClicked) {
    if (!isEnabled) return false;
    return isHovered && mouseClicked;
}

void Button::setEnabled(bool enabled) {
    isEnabled = enabled;
}

void Button::setPosition(float x, float y) {
    shape.setPosition(x, y);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setPosition(x + shape.getSize().x / 2.f, y + shape.getSize().y / 2.f);
}

void Button::setText(const std::string& newText) {
    text.setString(newText);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
    text.setPosition(shape.getPosition().x + shape.getSize().x / 2.f, shape.getPosition().y + shape.getSize().y / 2.f);
}

ChipButton::ChipButton(float x, float y, int value, const std::string& label, sf::Font& font, sf::Texture& texture, sf::IntRect textureRect)
    : chipValue(value), isHovered(false), isEnabled(true) {
    sprite.setTexture(texture);
    sprite.setTextureRect(textureRect);
    sprite.setPosition(x, y);

    text.setFont(font);
    text.setString(label);
    text.setCharacterSize(14);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.5f);

    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
    text.setPosition(x + 32.f, y + 36.f);
}

void ChipButton::update(const sf::Vector2f& mousePos) {
    if (!isEnabled) {
        sprite.setColor(sf::Color(100, 100, 100, 150));
        text.setFillColor(sf::Color(150, 150, 150, 150));
        return;
    }

    if (sprite.getGlobalBounds().contains(mousePos)) {
        isHovered = true;
        sprite.setColor(sf::Color(255, 255, 255, 255));
        text.setFillColor(sf::Color(255, 235, 100, 255));
    } else {
        isHovered = false;
        sprite.setColor(sf::Color(210, 210, 210, 255));
        text.setFillColor(sf::Color::White);
    }
}

void ChipButton::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    window.draw(text);
}

bool ChipButton::isPressed(const sf::Vector2f& mousePos, bool mouseClicked) {
    if (!isEnabled) return false;
    return isHovered && mouseClicked;
}

void ChipButton::setEnabled(bool enabled) {
    isEnabled = enabled;
}

void ChipButton::setTexture(const sf::Texture& texture) {
    sprite.setTexture(texture);
}
