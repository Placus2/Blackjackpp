#pragma once
#include "Card.h"
#include <vector>

class Deck {
private:
    std::vector<Card> cards;
    sf::Sprite deckSprite;
public:
    Deck();
    void reset();
    void shuffle();
    Card drawCard();
    void draw(sf::RenderWindow& window);
};
