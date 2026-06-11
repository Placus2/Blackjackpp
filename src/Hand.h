#pragma once
#include "Card.h"
#include "GameObject.h"
#include <vector>

class Hand : public GameObject {
public:
    std::vector<Card> cards;
    sf::Vector2f startPos;

    bool hideFirstCard = false;
    bool transparentFirstCard = false;

    Hand(sf::Vector2f position);
    void addCard(Card c);
    void updateCardPositions();
    int getTotal() const;

    void draw(sf::RenderWindow& window) override;
    

    void update(float dt) override;
    

    void draw(sf::RenderWindow& window, bool hideFirstCardVal, bool transparentFirstCardVal);
    
    bool shouldDraw(GameState state) const override;
};
