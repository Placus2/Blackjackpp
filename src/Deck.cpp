#include "Deck.h"
#include "TextureManager.h"
#include <random>
#include <algorithm>

Deck::Deck() : cardBackPath("textures/cardback/cardBackRed.png") {
    deckSprite.setTexture(texManager.get(cardBackPath));
    deckSprite.setPosition(730.f, 150.f);
    deckSprite.setScale(0.15f, 0.15f);
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
            c.texturePath = path;
            c.frontSprite.setTexture(texManager.get(path));
            c.frontSprite.setScale(0.15f, 0.15f);
            c.backSprite.setTexture(texManager.get(cardBackPath));
            c.backSprite.setScale(0.15f, 0.15f);

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
    deactivatePeek();
    c.currentPos = deckSprite.getPosition();
    c.frontSprite.setPosition(c.currentPos);
    c.backSprite.setPosition(c.currentPos);
    return c;
}
void Deck::activatePeek(){
    if (cards.empty()) return;
    peekSprite = cards.back().frontSprite;
    sf::Vector2f deckPos = deckSprite.getPosition();
    peekSprite.setPosition(deckPos.x - 9.f, deckPos.y - 11.f);
    peekSprite.setScale(0.15f, 0.15f);
    peekSprite.setColor(sf::Color(255, 255, 255, 180));
    isPeeking = true;
}
void Deck::deactivatePeek(){
    isPeeking = false;
}
std::string Deck::getPeekDescription() const {
    if (!isPeeking || cards.empty()) return "";
    return cards.back().rank;
}
void Deck::draw(sf::RenderWindow& window) {
    if (!cards.empty()) {
        window.draw(deckSprite);
        if (isPeeking) {
            window.draw(peekSprite);
        }
    }
}

void Deck::setCardBackPath(const std::string& path) {
    cardBackPath = path;
    deckSprite.setTexture(texManager.get(cardBackPath));
}
