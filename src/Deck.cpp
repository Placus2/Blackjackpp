#include "Deck.h"
#include "TextureManager.h"
#include <random>
#include <algorithm>

Deck::Deck() {
    deckSprite.setTexture(texManager.get("textures/cardback/cardBackRed.png"));
    deckSprite.setPosition(850.f, 320.f);
    deckSprite.setScale(0.3f, 0.3f);
}

void Deck::reset() {
    cards.clear();
    std::string folders[] = {"club", "diamond", "heart", "spade"};
    std::string suits[] = {"Clubs", "Diamonds", "Hearts", "Spades"};
    std::string ranks[] = {"2","3","4","5","6","7","8","9","10","J","Q","K","A"};
    int values[] = {2,3,4,5,6,7,8,9,10,10,10,10,11};

    for (int s = 0; s < 4; ++s) {
        for (int r = 0; r < 13; ++r) {
            Card c;
            c.value = values[r];
            c.rank = ranks[r];

            std::string path = "textures/" + folders[s] + "/card" + suits[s] + "_" + ranks[r] + ".png";
            c.frontSprite.setTexture(texManager.get(path));
            c.frontSprite.setScale(0.3f, 0.3f);
            c.backSprite.setTexture(texManager.get("textures/cardback/cardBackRed.png"));
            c.backSprite.setScale(0.3f, 0.3f);

            cards.push_back(c);
        }
    }
    shuffle();
}

void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}

Card Deck::drawCard() {
    Card c = cards.back();
    cards.pop_back();
    c.currentPos = deckSprite.getPosition();
    c.frontSprite.setPosition(c.currentPos);
    c.backSprite.setPosition(c.currentPos);
    return c;
}

void Deck::draw(sf::RenderWindow& window) {
    if (!cards.empty()) window.draw(deckSprite);
}
