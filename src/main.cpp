#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "TextureManager.h"
#include "Card.h"
#include "Hand.h"
#include "Deck.h"

enum GameState { MENU, BETTING, PLAYER_TURN, DEALER_TURN, GAME_OVER };

int main() {
    sf::RenderWindow window(sf::VideoMode(1024, 768), "BLACKJACK PP");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("textures/arial.ttf")) {
        std::cerr << "Blad: Nie znaleziono textures/arial.ttf!" << std::endl;
        return -1;
    }

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(texManager.get("textures/Backgrounds/background_2.png"));
    backgroundSprite.setScale(1024.f / 934.f, 768.f / 523.f);

    Deck deck;
    Hand dealerHand(sf::Vector2f(350.f, 150.f));
    std::vector<Hand> playerHands;

    GameState state = MENU;
    std::string resultMessage = "";
    bool isHardMode = false;

    int balance = 10000;
    int currentBet = 0;
    int activeHandIndex = 0;
    bool lifelineUsed = false;
    bool peekUsed = false;

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
        lifelineUsed = false;
        peekUsed = false;

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
                    if (event.key.code == sf::Keyboard::Num6 && balance >= 500) { currentBet += 500; balance -= 500; }
                    if (event.key.code == sf::Keyboard::Num7 && balance >= 1000) { currentBet += 1000; balance -= 1000; }

                    if (event.key.code == sf::Keyboard::Backspace) {
                        balance += currentBet;
                        currentBet = 0;
                    }
                    if (event.key.code == sf::Keyboard::Escape){
                        state = MENU;
                    }
                    if (event.key.code == sf::Keyboard::Enter && currentBet > 0) {
                        startRound();
                    }

                    if (event.key.code == sf::Keyboard::B && balance == 0 && currentBet == 0) {
                        balance = 10000;
                    }
                }
                else if (state == PLAYER_TURN && !isAnyAnimating) {
                    Hand& currentHand = playerHands[activeHandIndex];
                    bool canSplit = (playerHands.size() == 1 && currentHand.cards.size() == 2 &&
                                     currentHand.cards[0].value == currentHand.cards[1].value &&
                                     balance >= currentBet);

                    if (event.key.code == sf::Keyboard::H) {
                        currentHand.addCard(deck.drawCard());
                        if (currentHand.getTotal() > 21) {
                            if (lifelineUsed || balance < 25){
                                activeHandIndex++;
                            }
                        }
                    }
                    else if (event.key.code == sf::Keyboard::S) {
                        activeHandIndex++;
                    }
                    else if (event.key.code == sf::Keyboard::L && currentHand.getTotal() > 21 && !lifelineUsed && balance >=25){
                        balance -= 25;
                        lifelineUsed = true;
                        Card lifelineCard = deck.drawCard();
                        lifelineCard.isSubstracting = true;
                        currentHand.addCard(lifelineCard);
                        if (currentHand.getTotal() > 21){
                            activeHandIndex++;
                        }
                    }
                    else if (event.key.code == sf::Keyboard::K && !peekUsed && balance >= 25 && currentHand.getTotal() <= 21) {
                        balance -= 25;
                        peekUsed = true;
                        deck.activatePeek();
                    }
                    else if (canSplit && event.key.code == sf::Keyboard::P) {
                        balance -= currentBet;
                        currentBet *= 2;
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

        for (auto& c : dealerHand.cards) c.updateAnimation(dt);
        for (auto& h : playerHands)
            for (auto& c : h.cards) c.updateAnimation(dt);

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
                                      "[1] $1\n[2] $5\n[3] $10\n[4] $25\n[5] $100\n[6] $500\n[7] $1000\n\n"
                                      "[Backspace] Cofnij zaklad\n"
                                      "[Enter] Rozdaj Karty\n"
                                      "[Escape] Powrot \n"
                             + (balance == 0 && currentBet == 0 ? "\n[B] Bankructwo! Wcisnij by odzyskac $10000" : ""));
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
                    if (playerHands[activeHandIndex].getTotal() > 21){
                        statusStr += "PRZEKROCZONO 21 PUNKTOW! \n";
                        if (!lifelineUsed && balance >= 25){
                            statusStr += "[L] Kup kolo ratunkowe (-25$)\n";
                        }
                        statusStr += "[S] Akcjeptuj przegrana";
                    }else{
                         statusStr += "Akcja:\n[H] Dobierz\n[S] Czekaj\n";
                        if (!peekUsed && balance >= 25) {
                            statusStr += "[K] Podejrzyj nastepna karte (-$25)\n";
                        } else if (deck.getIsPeeking()) {
                            statusStr += "Nastepna karta: [ "
                                         + deck.getPeekDescription()
                                         + " ]\n";
                        }
                    if (playerHands.size() == 1 && playerHands[0].cards.size() == 2 &&
                        playerHands[0].cards[0].value == playerHands[0].cards[1].value && balance >= currentBet) {
                        statusStr += "[P] Podziel (-$" + std::to_string(currentBet) + ")\n";
                        }
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
