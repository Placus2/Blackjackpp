#include "Deck.h"
#include "TextureManager.h"
#include <random>
#include <algorithm>

// Konstruktor klasy reprezentujacej talie kart
Deck::Deck() : cardBackPath("textures/cardback/cardBackRed.png") {
    deckSprite.setTexture(texManager.get(cardBackPath));
    deckSprite.setPosition(730.f, 150.f);
    deckSprite.setScale(0.15f, 0.15f);
}

// Resetuje talie, tworzac 52 nowe karty i tasujac je
void Deck::reset() {
    isPeeking = false;
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

// Tasuje losowo karty w talii
void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}

// Wyciaga karte z gory talii
Card Deck::drawCard() {
    Card c = cards.back();
    cards.pop_back();
    deactivatePeek();
    c.currentPos = deckSprite.getPosition();
    c.frontSprite.setPosition(c.currentPos);
    c.backSprite.setPosition(c.currentPos);
    return c;
}

// Aktywuje podglądanie nastepnej karty w talii
void Deck::activatePeek(){
    if (cards.empty()) return;
    peekSprite = cards.back().frontSprite;
    sf::Vector2f deckPos = deckSprite.getPosition();
    peekSprite.setPosition(deckPos.x - 9.f, deckPos.y - 11.f);
    peekSprite.setScale(0.15f, 0.15f);
    peekSprite.setColor(sf::Color(255, 255, 255, 180));
    isPeeking = true;
}

// Wylacza podglądanie karty
void Deck::deactivatePeek(){
    isPeeking = false;
}

// Zwraca opis podgladanej karty (figure/wartosc)
std::string Deck::getPeekDescription() const {
    if (!isPeeking || cards.empty()) return "";
    return cards.back().rank;
}

// Rysuje talie oraz ewentualny podglad karty na oknie gry
void Deck::draw(sf::RenderWindow& window) {
    if (!cards.empty()) {
        window.draw(deckSprite);
        if (isPeeking) {
            window.draw(peekSprite);
        }
    }
}

// Ustawia sciezke do tekstury rewersu kart
void Deck::setCardBackPath(const std::string& path) {
    cardBackPath = path;
    deckSprite.setTexture(texManager.get(cardBackPath));
}

// Okresla, czy talia powinna byc rysowana w danym stanie
bool Deck::shouldDraw(GameState state) const {
    return state != MENU && state != GAMEPLAY_OPTIONS && state != SETTINGS && state != LEADERBOARD && state != PROFILE_SELECT && state != PROFILE_CREATE && state != GAME_OVER && state != BETTING;
}
