#include "Button.h"

// Konstruktor zwyklego przycisku z tekstem
Button::Button(float x, float y, float width, float height,
               const std::string& buttonText, sf::Font& font, unsigned int characterSize) {
    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(width, height));
    
    // KOLORYSTYKA
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

    // Centrowanie tekstu
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
    text.setPosition(x + width / 2.f, y + height / 2.f);

    isHovered = false;
    isEnabled = true;
    hasDrawState = false;
}

// Ustawia stan gry, w ktorym przycisk powinien byc rysowany
void Button::setDrawState(GameState state) {
    drawState = state;
    hasDrawState = true;
}

// Sprawdza, czy przycisk powinien byc wyswietlany w danym stanie
bool Button::shouldDraw(GameState state) const {
    if (!isVisible) return false;
    if (hasDrawState) {
        return state == drawState;
    }
    return false;
}

// Aktualizuje stan przycisku (kolor, ramke) na podstawie pozycji myszy
void Button::updateMouse(const sf::Vector2f& mousePos) {
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

// Rysuje przycisk i jego tekst na oknie
void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

// Sprawdza, czy przycisk zostal klikniety
bool Button::isPressed(const sf::Vector2f& mousePos, bool mouseClicked) {
    if (!isEnabled) return false;
    return isHovered && mouseClicked;
}

// Ustawia czy przycisk jest aktywny/interaktywny
void Button::setEnabled(bool enabled) {
    isEnabled = enabled;
}

// Ustawia widocznosc przycisku
void Button::setVisible(bool visible) {
    isVisible = visible;
}

// Zmienia pozycje przycisku i centruje jego tekst
void Button::setPosition(float x, float y) {
    shape.setPosition(x, y);
    text.setPosition(x + shape.getSize().x / 2.f, y + shape.getSize().y / 2.f);
}

// Zmienia tekst przycisku i ponownie go centruje
void Button::setText(const std::string& newText) {
    text.setString(newText);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
    text.setPosition(shape.getPosition().x + shape.getSize().x / 2.f, shape.getPosition().y + shape.getSize().y / 2.f);
}

// Konstruktor przycisku zetonu z tekstura i wartoscia
ChipButton::ChipButton(float x, float y, int value, const std::string& label, sf::Font& font, sf::Texture& texture, sf::IntRect textureRect)
    : Button(x, y, 64.f, 72.f, "", font), chipValue(value) {
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

    setDrawState(BETTING);
}

// Rysuje zeton i jego wartosc na oknie
void ChipButton::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    window.draw(text);
}

// Aktualizuje stan podswietlenia zetonu na podstawie pozycji myszy
void ChipButton::updateMouse(const sf::Vector2f& mousePos) {
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

// Sprawdza, czy zeton zostal klikniety
bool ChipButton::isPressed(const sf::Vector2f& mousePos, bool mouseClicked) {
    if (!isEnabled) return false;
    return isHovered && mouseClicked;
}

// Ustawia teksture zetonu
void ChipButton::setTexture(const sf::Texture& texture) {
    sprite.setTexture(texture);
}
