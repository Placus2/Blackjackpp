#include "Hand.h"

Hand::Hand(sf::Vector2f position) : startPos(position) {}

void Hand::addCard(Card c) {
    cards.push_back(c);
    updateCardPositions();
}

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

void Hand::draw(sf::RenderWindow& window) {
    draw(window, hideFirstCard, transparentFirstCard);
}
// RYSOWANIE KART
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

// OKRESLANIE WIDOCZNOSCI TALII
bool Hand::shouldDraw(GameState state) const {
    return state != MENU && state != GAMEPLAY_OPTIONS && state != SETTINGS && state != LEADERBOARD && state != BETTING && state != PROFILE_SELECT && state != PROFILE_CREATE;
}

// AKTUALIZACJA ANIMACJI KART
void Hand::update(float dt) {
    for (auto& c : cards) {
        c.updateAnimation(dt);
    }
}

