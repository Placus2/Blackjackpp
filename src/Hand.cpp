#include "Hand.h"

// Konstruktor klasy reprezentujacej reke gracza lub krupiera
Hand::Hand(sf::Vector2f position) : startPos(position) {}

// Dodaje karte do reki i aktualizuje pozycje animacji
void Hand::addCard(Card c) {
    cards.push_back(c);
    updateCardPositions();
}

// Aktualizuje docelowe pozycje kart w rece, dopasowujac odstepy
void Hand::updateCardPositions() {
    float spacing = 60.f;
    float max_width = 150.f;
    if (cards.size() > 1) {
        float calculated_spacing = max_width / (cards.size() - 1);
        if (calculated_spacing < spacing) {
            spacing = calculated_spacing;
        }
    }

    for (size_t i = 0; i < cards.size(); ++i) {
        cards[i].targetPos = sf::Vector2f(startPos.x + i * spacing, startPos.y);
        cards[i].isAnimating = true;
    }
}

// Oblicza sume punktow kart w rece, uwzgledniajac asy i karty odejmujace
int Hand::getTotal() const {
    int sum = 0, aces = 0;
    for (const auto& c : cards) {
        if (c.isSubstracting){
            sum -= c.value;
        }else{
            sum += c.value;
            if (c.rank == "A") aces++;
        }
    }
    while (sum > 21 && aces > 0) {
        sum -= 10;
        aces--;
    }
    return sum;
}

// Rysuje karty w rece na oknie gry
void Hand::draw(sf::RenderWindow& window) {
    draw(window, hideFirstCard, transparentFirstCard);
}

// Rysuje karty z opcjami ukrycia lub polprzezroczystosci pierwszej karty
void Hand::draw(sf::RenderWindow& window, bool hideFirstCardVal, bool transparentFirstCardVal) {
    for (size_t i = 0; i < cards.size(); ++i) {
        if (i == 0 && hideFirstCardVal) {
            window.draw(cards[i].backSprite);
        } else {
            if (i == 0 && transparentFirstCardVal) {
                sf::Color originalColor = cards[i].frontSprite.getColor();
                cards[i].frontSprite.setColor(sf::Color(255, 255, 255, 180));
                window.draw(cards[i].frontSprite);
                cards[i].frontSprite.setColor(originalColor);
            } else {
                window.draw(cards[i].frontSprite);
            }
        }
    }
}

// Okresla, czy reka powinna byc rysowana w danym stanie gry
bool Hand::shouldDraw(GameState state) const {
    return state != MENU && state != GAMEPLAY_OPTIONS && state != SETTINGS && state != LEADERBOARD && state != BETTING && state != PROFILE_SELECT && state != PROFILE_CREATE;
}

// Aktualizuje animacje ruchu wszystkich kart w rece
void Hand::update(float dt) {
    for (auto& c : cards) {
        c.updateAnimation(dt);
    }
}

