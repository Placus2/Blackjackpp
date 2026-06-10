#include "Hand.h"

Hand::Hand(sf::Vector2f position) : startPos(position) {}

void Hand::addCard(Card c) {
    cards.push_back(c);
    updateCardPositions();
}

void Hand::updateCardPositions() {
    float spacing = (cards.size() > 2) ? 100.f : 140.f;
    float shiftX = (cards.size() > 2) ? (cards.size() - 2) * 30.f : 0.f;

    for (size_t i = 0; i < cards.size(); ++i) {
        float xOffset = i * spacing;
        cards[i].targetPos = sf::Vector2f(startPos.x + xOffset - shiftX, startPos.y);
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

void Hand::draw(sf::RenderWindow& window, bool hideFirstCard) {
    for (size_t i = 0; i < cards.size(); ++i) {
        if (i == 0 && hideFirstCard) {
            window.draw(cards[i].backSprite);
        } else {
            window.draw(cards[i].frontSprite);
        }
    }
}
