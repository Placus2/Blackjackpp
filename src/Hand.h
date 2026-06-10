#pragma once
#include "Card.h"
#include <vector>

class Hand {
public:
    std::vector<Card> cards;
    sf::Vector2f startPos;

    Hand(sf::Vector2f position);
    void addCard(Card c);
    void updateCardPositions();
    int getTotal() const;
    void draw(sf::RenderWindow& window, bool hideFirstCard = false);
};
