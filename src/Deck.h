#pragma once
#include "Card.h"
#include <vector>

class Deck {
private:
    std::vector<Card> cards;
    sf::Sprite deckSprite;
    bool isPeeking = false;
    sf::Sprite peekSprite;
public:
    Deck();
    void reset();
    void shuffle();
    Card drawCard();
    void draw(sf::RenderWindow& window);
    void activatePeek();
    void deactivatePeek();
    std::string getPeekDescription() const;
    bool getIsPeeking() const {return isPeeking; }
};
