#pragma once
#include "Card.h"
#include "GameObject.h"
#include <vector>


class Deck : public GameObject {
private:
    std::vector<Card> cards;
    sf::Sprite deckSprite;
    bool isPeeking = false;
    sf::Sprite peekSprite;
    std::string cardBackPath;
public:
    Deck();
    void reset();
    void shuffle();
    Card drawCard();
    

    void draw(sf::RenderWindow& window) override;
    

    bool shouldDraw(GameState state) const override;

    void activatePeek();
    void deactivatePeek();
    std::string getPeekDescription() const;
    bool getIsPeeking() const { return isPeeking; }
    
    void setCardBackPath(const std::string& path);
    std::string getCardBackPath() const { return cardBackPath; }
};
