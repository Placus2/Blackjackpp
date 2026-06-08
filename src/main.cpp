#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <map>
#include <cmath>

// --- MENEDŻER TEKSTUR ---
class TextureManager {
private:
    std::map<std::string, sf::Texture> textures;
public:
    sf::Texture& get(const std::string& path) {
        if (textures.find(path) == textures.end()) {
            if (!textures[path].loadFromFile(path)) {
                std::cerr << "Blad ladowania tekstury: " << path << std::endl;
            }
        }
        return textures[path];
    }
};

TextureManager texManager;

// --- STRUKTURY I KLASY ---
struct Card {
    int value;
    std::string rank;
    sf::Sprite frontSprite;
    sf::Sprite backSprite;

    sf::Vector2f currentPos;
    sf::Vector2f targetPos;
    bool isAnimating = false;

    void updateAnimation(float dt) {
        if (!isAnimating) return;

        sf::Vector2f dir = targetPos - currentPos;
        float distance = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        float speed = 2500.f;

        if (distance < speed * dt) {
            currentPos = targetPos;
            isAnimating = false;
        } else {
            currentPos += (dir / distance) * speed * dt;
        }

        frontSprite.setPosition(currentPos);
        backSprite.setPosition(currentPos);
    }
};

class Hand {
public:
    std::vector<Card> cards;
    sf::Vector2f startPos;

    Hand(sf::Vector2f position) : startPos(position) {}

    void addCard(Card c) {
        cards.push_back(c);
        updateCardPositions();
    }

    void updateCardPositions() {
        for (size_t i = 0; i < cards.size(); ++i) {
            float xOffset = i * 120.f;
            cards[i].targetPos = sf::Vector2f(startPos.x + xOffset, startPos.y);
            cards[i].isAnimating = true;
        }
    }

    int getTotal() const {
        int sum = 0, aces = 0;
        for (const auto& c : cards) {
            sum += c.value;
            if (c.rank == "A") aces++;
        }
        while (sum > 21 && aces > 0) {
            sum -= 10;
            aces--;
        }
        return sum;
    }

    void draw(sf::RenderWindow& window, bool hideFirstCard = false) {
        for (size_t i = 0; i < cards.size(); ++i) {
            if (i == 0 && hideFirstCard) {
                window.draw(cards[i].backSprite);
            } else {
                window.draw(cards[i].frontSprite);
            }
        }
    }
};

class Deck {
private:
    std::vector<Card> cards;
    sf::Sprite deckSprite;
public:
    Deck() {
        deckSprite.setTexture(texManager.get("textures/cardback/cardBackBlue.png"));
        deckSprite.setPosition(850.f, 320.f);
        deckSprite.setScale(0.3f, 0.3f);
    }

    void reset() {
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
                c.backSprite.setTexture(texManager.get("textures/cardback/cardBackBlue.png"));
                c.backSprite.setScale(0.3f, 0.3f);

                cards.push_back(c);
            }
        }
        shuffle();
    }

    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(cards.begin(), cards.end(), g);
    }

    Card drawCard() {
        Card c = cards.back();
        cards.pop_back();
        c.currentPos = deckSprite.getPosition();
        c.frontSprite.setPosition(c.currentPos);
        c.backSprite.setPosition(c.currentPos);
        return c;
    }

    void draw(sf::RenderWindow& window) {
        if (!cards.empty()) window.draw(deckSprite);
    }
};

// --- LOGIKA GRY ---
enum GameState { MENU, BETTING, PLAYER_TURN, DEALER_TURN, GAME_OVER };

int main() {
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Wizualny Blackjack");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Blad: Nie znaleziono textures/arial.ttf!" << std::endl;
        return -1;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(texManager.get("textures/Backgrounds/background_1.png"));
    backgroundSprite.setScale(1024.f / 934.f, 768.f / 523.f);

    Deck deck;
    Hand dealerHand(sf::Vector2f(350.f, 150.f));
    std::vector<Hand> playerHands;

    GameState state = MENU;
    std::string resultMessage = "";
    bool isHardMode = false;

    // Ekonomia gry
    int balance = 100;
    int currentBet = 0;
    int activeHandIndex = 0;

    sf::Text uiText("", font, 20);
    uiText.setFillColor(sf::Color::White);
    uiText.setOutlineColor(sf::Color::Black);
    uiText.setOutlineThickness(2.f);

    sf::Clock clock;
    float dealerTimer = 0.f;

    auto startRound = [&]() {
        deck.reset();
        playerHands.clear();
        dealerHand.cards.clear();

        playerHands.push_back(Hand(sf::Vector2f(350.f, 500.f)));

        playerHands[0].addCard(deck.drawCard());
        dealerHand.addCard(deck.drawCard());
        playerHands[0].addCard(deck.drawCard());
        dealerHand.addCard(deck.drawCard());

        activeHandIndex = 0;
        state = PLAYER_TURN;
        resultMessage = "";
        dealerTimer = 0.f;
    };

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Event event;

        bool isAnyAnimating = false;
        for (const auto& c : dealerHand.cards) if (c.isAnimating) isAnyAnimating = true;
        for (const auto& h : playerHands)
            for (const auto& c : h.cards) if (c.isAnimating) isAnyAnimating = true;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed) {
                if (state == MENU) {
                    if (event.key.code == sf::Keyboard::Num1) { isHardMode = false; state = BETTING; }
                    else if (event.key.code == sf::Keyboard::Num2) { isHardMode = true; state = BETTING; }
                }
                else if (state == BETTING) {
                    if (event.key.code == sf::Keyboard::Num1 && balance >= 1) { currentBet += 1; balance -= 1; }
                    if (event.key.code == sf::Keyboard::Num2 && balance >= 5) { currentBet += 5; balance -= 5; }
                    if (event.key.code == sf::Keyboard::Num3 && balance >= 10) { currentBet += 10; balance -= 10; }
                    if (event.key.code == sf::Keyboard::Num4 && balance >= 25) { currentBet += 25; balance -= 25; }
                    if (event.key.code == sf::Keyboard::Num5 && balance >= 100) { currentBet += 100; balance -= 100; }

                    if (event.key.code == sf::Keyboard::Backspace) {
                        balance += currentBet;
                        currentBet = 0;
                    }

                    if (event.key.code == sf::Keyboard::Enter && currentBet > 0) {
                        startRound();
                    }

                    if (event.key.code == sf::Keyboard::B && balance == 0 && currentBet == 0) {
                        balance = 100;
                    }
                }
                else if (state == PLAYER_TURN && !isAnyAnimating) {
                    Hand& currentHand = playerHands[activeHandIndex];
                    bool canSplit = (playerHands.size() == 1 && currentHand.cards.size() == 2 &&
                                     currentHand.cards[0].value == currentHand.cards[1].value &&
                                     balance >= currentBet);

                    if (event.key.code == sf::Keyboard::H) {
                        currentHand.addCard(deck.drawCard());
                        if (currentHand.getTotal() > 21) activeHandIndex++;
                    }
                    else if (event.key.code == sf::Keyboard::S) {
                        activeHandIndex++;
                    }
                    else if (canSplit && event.key.code == sf::Keyboard::P) {
                        balance -= currentBet;

                        Card splitCard = currentHand.cards.back();
                        currentHand.cards.pop_back();

                        currentHand.startPos = sf::Vector2f(150.f, 500.f);
                        currentHand.updateCardPositions();

                        Hand secondHand(sf::Vector2f(550.f, 500.f));
                        splitCard.currentPos = currentHand.cards[0].currentPos;
                        secondHand.addCard(splitCard);

                        playerHands.push_back(secondHand);
                    }

                    if (activeHandIndex >= playerHands.size()) {
                        bool allBusted = true;
                        for (const auto& h : playerHands) if (h.getTotal() <= 21) allBusted = false;
                        state = allBusted ? GAME_OVER : DEALER_TURN;
                    }
                }
                else if (state == GAME_OVER && !isAnyAnimating) {
                    if (event.key.code == sf::Keyboard::R) {
                        currentBet = 0;
                        state = BETTING;
                    }
                }
            }
        }

        // --- AKTUALIZACJA ANIMACJI ---
        for (auto& c : dealerHand.cards) c.updateAnimation(dt);
        for (auto& h : playerHands)
            for (auto& c : h.cards) c.updateAnimation(dt);

        // --- LOGIKA KRUPIERA ---
        if (state == DEALER_TURN && !isAnyAnimating) {
            dealerTimer += dt;
            if (dealerTimer >= 0.8f) {
                dealerTimer = 0.f;
                int dealerLimit = isHardMode ? 18 : 17;
                if (dealerHand.getTotal() < dealerLimit) {
                    dealerHand.addCard(deck.drawCard());
                } else {
                    state = GAME_OVER;
                }
            }
        }

        // --- ROZLICZENIE ZAKŁADÓW ---
        if (state == GAME_OVER && resultMessage.empty()) {
            int dTotal = dealerHand.getTotal();

            for (size_t i = 0; i < playerHands.size(); ++i) {
                int pTotal = playerHands[i].getTotal();
                bool isBlackjack = (playerHands.size() == 1 && playerHands[i].cards.size() == 2 && pTotal == 21);
                std::string prefix = playerHands.size() > 1 ? ("Reka " + std::to_string(i + 1) + ": ") : "";

                if (pTotal > 21) {
                    resultMessage += prefix + "Spalona.\n";
                } else if (isBlackjack && dTotal != 21) {
                    resultMessage += prefix + "BLACKJACK!\n";
                    balance += currentBet * 2.5;
                } else if (dTotal > 21) {
                    resultMessage += prefix + "Krupier spalil! Wygrywasz!\n";
                    balance += currentBet * 2;
                } else if (pTotal > dTotal) {
                    resultMessage += prefix + "Wygrywasz!\n";
                    balance += currentBet * 2;
                } else if (dTotal > pTotal) {
                    resultMessage += prefix + "Przegrywasz.\n";
                } else {
                    resultMessage += prefix + "Remis.\n";
                    balance += currentBet;
                }
            }
        }

        // --- RYSOWANIE ---
        window.clear();
        window.draw(backgroundSprite);

        std::string ecoStr = "Saldo: $" + std::to_string(balance) + "\nZaklad: $" + std::to_string(currentBet) + "\n\n";

        if (state == MENU) {
            uiText.setString("--- WYBIERZ POZIOM TRUDNOSCI ---\n\n"
                             "[ 1 ] NORMALNY\n"
                             "[ 2 ] TRUDNY");
            uiText.setPosition(350.f, 300.f);
            window.draw(uiText);
        }
        else if (state == BETTING) {
            uiText.setString(ecoStr + "ZAMIAN ZETONOW:\n"
                                      "[1] $1\n[2] $5\n[3] $10\n[4] $25\n[5] $100\n\n"
                                      "[Backspace] Cofnij zaklad\n"
                                      "[Enter] Rozdaj Karty\n"
                             + (balance == 0 && currentBet == 0 ? "\n[B] Bankructwo! Wcisnij by odzyskac $100" : ""));
            uiText.setPosition(350.f, 250.f);
            window.draw(uiText);
        }
        else {
            deck.draw(window);
            dealerHand.draw(window, state == PLAYER_TURN);
            for (auto& hand : playerHands) hand.draw(window);

            std::string statusStr = ecoStr + "Krupier:\nSuma: ";
            if (state != PLAYER_TURN) statusStr += std::to_string(dealerHand.getTotal()) + "\n";
            else statusStr += "?\n";

            statusStr += "\nTy:\n";
            for (size_t i = 0; i < playerHands.size(); ++i) {
                if (playerHands.size() > 1) statusStr += "Reka " + std::to_string(i + 1) + " ";
                if (state == PLAYER_TURN && i == activeHandIndex) statusStr += "(Aktywna) ";
                statusStr += "Suma: " + std::to_string(playerHands[i].getTotal()) + "\n";
            }

            statusStr += "\n";

            if (state == PLAYER_TURN) {
                if (isAnyAnimating) {
                    statusStr += "Rozdawanie...";
                } else {
                    statusStr += "Akcja:\n[H] Dobierz\n[S] Czekaj\n";
                    if (playerHands.size() == 1 && playerHands[0].cards.size() == 2 &&
                        playerHands[0].cards[0].value == playerHands[0].cards[1].value && balance >= currentBet) {
                        statusStr += "[P] Podziel (-$" + std::to_string(currentBet) + ")\n";
                    }
                }
            } else if (state == GAME_OVER) {
                statusStr += "WYNIK:\n" + resultMessage + "\n[R] Nowe rozdanie";
            } else if (state == DEALER_TURN) {
                statusStr += "Tura Krupiera...";
            }

            uiText.setString(statusStr);
            uiText.setPosition(30.f, 150.f);
            window.draw(uiText);
        }

        window.display();
    }

    return 0;
}
