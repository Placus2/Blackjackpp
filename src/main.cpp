#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include "TextureManager.h"
#include "Card.h"
#include "Hand.h"
#include "Deck.h"
#include "Button.h"

enum GameState { MENU, GAMEPLAY_OPTIONS, BETTING, DEALING_START, PLAYER_TURN, DEALER_TURN, GAME_OVER, SETTINGS };

sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight) {
    float windowRatio = (float)windowWidth / (float)windowHeight;
    float viewRatio = (float)view.getSize().x / (float)view.getSize().y;
    float sizeX = 1.f;
    float sizeY = 1.f;
    float posX = 0.f;
    float posY = 0.f;

    bool horizontalSpacing = true;
    if (windowRatio < viewRatio)
        horizontalSpacing = false;

    if (horizontalSpacing) {
        sizeX = viewRatio / windowRatio;
        posX = (1.f - sizeX) / 2.f;
    } else {
        sizeY = windowRatio / viewRatio;
        posY = (1.f - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
    return view;
}

void saveGame(int balance, const std::string& bgPath, const std::string& tablePath, const std::string& cbPath, bool isFullscreen) {
    std::filesystem::create_directories("Save");
    std::ofstream file("Save/savegame.txt");
    if (file.is_open()) {
        file << balance << "\n";
        file << bgPath << "\n";
        file << tablePath << "\n";
        file << cbPath << "\n";
        file << (isFullscreen ? 1 : 0) << "\n";
        file.close();
    }
}

void loadGame(int& balance, std::string& bgPath, std::string& tablePath, std::string& cbPath, bool& isFullscreen) {
    std::ifstream file("Save/savegame.txt");
    bool success = false;
    if (file.is_open()) {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            lines.push_back(line);
        }
        file.close();
        if (lines.size() >= 5) {
            try {
                balance = std::stoi(lines[0]);
                bgPath = lines[1];
                tablePath = lines[2];
                cbPath = lines[3];
                isFullscreen = (std::stoi(lines[4]) != 0);
                success = true;
            } catch (...) {
                success = false;
            }
        }
    }
    if (!success) {
        balance = 1000;
        bgPath = "textures/Backgrounds/background_1.png";
        tablePath = "textures/Tables/table_green.png.png";
        cbPath = "textures/cardback/cardBackRed.png";
        isFullscreen = false;
        saveGame(balance, bgPath, tablePath, cbPath, isFullscreen);
    }
}

int main() {
    int balance = 1000;
    std::string currentBgPath = "textures/Backgrounds/background_1.png";
    std::string currentTablePath = "textures/Tables/table_green.png.png";
    std::string currentCbPath = "textures/cardback/cardBackRed.png";
    bool isFullscreen = false;

    loadGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);

    sf::SoundBuffer fanBuffer;
    sf::Sound fanSound;
    if (fanBuffer.loadFromFile("sounds/card-fan-1.ogg")) {
        fanSound.setBuffer(fanBuffer);
    } else {
        std::cerr << "Error loading sound: sounds/card-fan-1.ogg" << std::endl;
    }

    sf::SoundBuffer placeBuffer;
    sf::Sound placeSound;
    if (placeBuffer.loadFromFile("sounds/card-place-1.ogg")) {
        placeSound.setBuffer(placeBuffer);
    } else {
        std::cerr << "Error loading sound: sounds/card-place-1.ogg" << std::endl;
    }

    sf::SoundBuffer handleBuffer;
    sf::Sound handleSound;
    if (handleBuffer.loadFromFile("sounds/chips-handle-6.ogg")) {
        handleSound.setBuffer(handleBuffer);
    } else {
        std::cerr << "Error loading sound: sounds/chips-handle-6.ogg" << std::endl;
    }

    sf::RenderWindow window;
    if (isFullscreen) {
        window.create(sf::VideoMode::getDesktopMode(), "BLACKJACK PP", sf::Style::Fullscreen);
    } else {
        window.create(sf::VideoMode(1024, 768), "BLACKJACK PP", sf::Style::Close);
    }
    window.setFramerateLimit(60);

    sf::View gameView(sf::FloatRect(0.f, 0.f, 1024.f, 768.f));
    gameView = getLetterboxView(gameView, window.getSize().x, window.getSize().y);
    window.setView(gameView);

    sf::Font font;
    if (!font.loadFromFile("textures/arial.ttf")) {
        std::cerr << "Error: textures/arial.ttf not found!" << std::endl;
        return -1;
    }

    sf::Sprite backgroundSprite;
    auto updateBgTexture = [&](const std::string& path) {
        sf::Texture& tex = texManager.get(path);
        backgroundSprite.setTexture(tex, true);
        sf::Vector2u texSize = tex.getSize();
        backgroundSprite.setScale(1024.f / texSize.x, 768.f / texSize.y);
    };
    updateBgTexture(currentBgPath);

    sf::Sprite tableSprite;
    auto updateTableTexture = [&](const std::string& path) {
        sf::Texture& tex = texManager.get(path);
        tableSprite.setTexture(tex, true);
        sf::Vector2u texSize = tex.getSize();
        tableSprite.setScale(1024.f / texSize.x, 768.f / texSize.y);
    };
    updateTableTexture(currentTablePath);

    Deck deck;
    deck.setCardBackPath(currentCbPath);

    Button playBtn(362.f, 300.f, 300.f, 50.f, "PLAY", font, 20);
    Button settingsBtn(362.f, 370.f, 300.f, 50.f, "SETTINGS", font, 20);
    Button exitBtn(362.f, 440.f, 300.f, 50.f, "EXIT GAME", font, 20);

    Button modeClassicBtn(272.f, 210.f, 230.f, 45.f, "Classic", font, 18);
    Button modeCustomBtn(522.f, 210.f, 230.f, 45.f, "Custom", font, 18);
    Button diffNormalBtn(272.f, 330.f, 230.f, 45.f, "Normal", font, 18);
    Button diffHardBtn(522.f, 330.f, 230.f, 45.f, "Hard", font, 18);
    Button startGameBtn(362.f, 460.f, 300.f, 50.f, "START GAME", font, 20);
    Button backToMenuFromOptionsBtn(362.f, 530.f, 300.f, 50.f, "BACK", font, 20);

    sf::Texture& chipsTex = texManager.get("textures/chips/Chips.png");

    ChipButton chip1(312.f, 260.f, 1, "1$", font, chipsTex, sf::IntRect(0, 0, 64, 72));
    ChipButton chip5(396.f, 260.f, 5, "5$", font, chipsTex, sf::IntRect(64, 0, 64, 72));
    ChipButton chip25(480.f, 260.f, 25, "25$", font, chipsTex, sf::IntRect(128, 0, 64, 72));
    ChipButton chip10(564.f, 260.f, 10, "10$", font, chipsTex, sf::IntRect(192, 0, 64, 72));
    ChipButton chip100(648.f, 260.f, 100, "100$", font, chipsTex, sf::IntRect(256, 0, 64, 72));

    ChipButton chip500(312.f, 342.f, 500, "500$", font, chipsTex, sf::IntRect(0, 72, 64, 72));
    ChipButton chip1000(396.f, 342.f, 1000, "1k$", font, chipsTex, sf::IntRect(64, 72, 64, 72));
    ChipButton chip5000(480.f, 342.f, 5000, "5k$", font, chipsTex, sf::IntRect(128, 72, 64, 72));
    ChipButton chip10000(564.f, 342.f, 10000, "10k$", font, chipsTex, sf::IntRect(192, 72, 64, 72));
    ChipButton chip25000(648.f, 342.f, 25000, "25k$", font, chipsTex, sf::IntRect(256, 72, 64, 72));

    Button resetBetBtn(242.f, 440.f, 160.f, 45.f, "Reset Bet", font, 18);
    Button dealBtn(422.f, 440.f, 180.f, 45.f, "Deal", font, 18);
    Button backToMenuBtn(622.f, 440.f, 160.f, 45.f, "Back", font, 18);
    Button bailoutBtn(412.f, 500.f, 200.f, 45.f, "Recover $1000", font, 18);

    Button hitBtn(132.f, 690.f, 120.f, 45.f, "Hit", font, 18);
    Button standBtn(272.f, 690.f, 120.f, 45.f, "Stand", font, 18);
    Button splitBtn(412.f, 690.f, 120.f, 45.f, "Split", font, 18);

    Button lifelineBtn(842.f, 170.f, 150.f, 45.f, "Minus card -25$", font, 14);
    Button peekBtn(842.f, 230.f, 150.f, 45.f, "Sneaky peeky -25$", font, 14);

    Button peekDeckBtn(842.f, 230.f, 150.f, 45.f, "Peek Deck", font, 16);
    Button peekDealerBtn(842.f, 290.f, 150.f, 45.f, "Peek Dealer", font, 16);
    Button cancelPeekBtn(842.f, 350.f, 150.f, 45.f, "Cancel", font, 16);

    Button newRoundBtn(387.f, 600.f, 250.f, 50.f, "New Round", font, 20);

    Button bg1Btn(350.f, 160.f, 150.f, 45.f, "[ ] Bg 1", font, 18);
    Button bg2Btn(524.f, 160.f, 150.f, 45.f, "[ ] Bg 2", font, 18);

    Button tableGreenBtn(272.f, 250.f, 150.f, 45.f, "[ ] Green", font, 18);
    Button tableBlueBtn(442.f, 250.f, 150.f, 45.f, "[ ] Blue", font, 18);
    Button tableRedBtn(612.f, 250.f, 150.f, 45.f, "[ ] Red", font, 18);

    Button cbRedBtn(272.f, 340.f, 150.f, 45.f, "[ ] Red", font, 18);
    Button cbBlueBtn(442.f, 340.f, 150.f, 45.f, "[ ] Blue", font, 18);
    Button cbGreenBtn(612.f, 340.f, 150.f, 45.f, "[ ] Green", font, 18);

    Button fullscreenBtn(362.f, 430.f, 300.f, 50.f, "[ ] Fullscreen", font, 18);
    Button resetProgressBtn(362.f, 500.f, 300.f, 50.f, "Reset Progress ($1000)", font, 18);
    Button backFromSettingsBtn(362.f, 570.f, 300.f, 50.f, "Back to Menu", font, 18);

    auto updateSettingsButtonLabels = [&]() {
        if (currentBgPath == "textures/Backgrounds/background_1.png") {
            bg1Btn.setText("[x] Bg 1");
            bg2Btn.setText("[ ] Bg 2");
        } else {
            bg1Btn.setText("[ ] Bg 1");
            bg2Btn.setText("[x] Bg 2");
        }

        if (currentTablePath == "textures/Tables/table_green.png.png") {
            tableGreenBtn.setText("[x] Green");
            tableBlueBtn.setText("[ ] Blue");
            tableRedBtn.setText("[ ] Red");
        } else if (currentTablePath == "textures/Tables/table_blue.png") {
            tableGreenBtn.setText("[ ] Green");
            tableBlueBtn.setText("[x] Blue");
            tableRedBtn.setText("[ ] Red");
        } else {
            tableGreenBtn.setText("[ ] Green");
            tableBlueBtn.setText("[ ] Blue");
            tableRedBtn.setText("[x] Red");
        }

        if (currentCbPath == "textures/cardback/cardBackRed.png") {
            cbRedBtn.setText("[x] Red");
            cbBlueBtn.setText("[ ] Blue");
            cbGreenBtn.setText("[ ] Green");
        } else if (currentCbPath == "textures/cardback/cardBackBlue.png") {
            cbRedBtn.setText("[ ] Red");
            cbBlueBtn.setText("[x] Blue");
            cbGreenBtn.setText("[ ] Green");
        } else {
            cbRedBtn.setText("[ ] Red");
            cbBlueBtn.setText("[ ] Blue");
            cbGreenBtn.setText("[x] Green");
        }

        if (isFullscreen) {
            fullscreenBtn.setText("[x] Fullscreen");
        } else {
            fullscreenBtn.setText("[ ] Fullscreen");
        }
    };
    updateSettingsButtonLabels();

    Hand dealerHand(sf::Vector2f(350.f, 150.f));
    std::vector<Hand> playerHands;

    GameState state = MENU;
    std::string resultMessage = "";
    bool isHardMode = false;
    bool hasSkillsMode = true;

    int currentBet = 0;
    int activeHandIndex = 0;
    bool lifelineUsed = false;
    bool peekUsed = false;
    bool isPeekingChoiceActive = false;
    bool isDealerCardRevealed = false;

    auto updateGameplayOptionsLabels = [&]() {
        if (hasSkillsMode) {
            modeClassicBtn.setText("[ ] Classic");
            modeCustomBtn.setText("[x] Custom");
        } else {
            modeClassicBtn.setText("[x] Classic");
            modeCustomBtn.setText("[ ] Custom");
        }

        if (isHardMode) {
            diffNormalBtn.setText("[ ] Normal");
            diffHardBtn.setText("[x] Hard");
        } else {
            diffNormalBtn.setText("[x] Normal");
            diffHardBtn.setText("[ ] Hard");
        }
    };
    updateGameplayOptionsLabels();

    auto toggleWindowMode = [&](bool fullscreen) {
        texManager.clear();
        if (fullscreen) {
            window.create(sf::VideoMode::getDesktopMode(), "BLACKJACK PP", sf::Style::Fullscreen);
        } else {
            window.create(sf::VideoMode(1024, 768), "BLACKJACK PP", sf::Style::Close);
        }
        window.setFramerateLimit(60);
        
        if (!font.loadFromFile("textures/arial.ttf")) {
            std::cerr << "Error: textures/arial.ttf not found after window reconstruction!" << std::endl;
        }

        updateBgTexture(currentBgPath);
        updateTableTexture(currentTablePath);
        deck.setCardBackPath(currentCbPath);

        sf::Texture& chipsTex = texManager.get("textures/chips/Chips.png");
        chip1.setTexture(chipsTex);
        chip5.setTexture(chipsTex);
        chip25.setTexture(chipsTex);
        chip10.setTexture(chipsTex);
        chip100.setTexture(chipsTex);
        chip500.setTexture(chipsTex);
        chip1000.setTexture(chipsTex);
        chip5000.setTexture(chipsTex);
        chip10000.setTexture(chipsTex);
        chip25000.setTexture(chipsTex);

        sf::View newView(sf::FloatRect(0.f, 0.f, 1024.f, 768.f));
        gameView = getLetterboxView(newView, window.getSize().x, window.getSize().y);
        window.setView(gameView);
    };

    sf::Text uiText("", font, 20);
    uiText.setFillColor(sf::Color::White);
    uiText.setOutlineColor(sf::Color::Black);
    uiText.setOutlineThickness(2.f);

    sf::Clock clock;
    float dealerTimer = 0.f;
    int dealStep = 0;
    float dealTimer = 0.f;

    auto startRound = [&]() {
        fanSound.play();
        deck.reset();
        playerHands.clear();
        dealerHand.cards.clear();
        lifelineUsed = false;
        peekUsed = false;
        isPeekingChoiceActive = false;
        isDealerCardRevealed = false;

        playerHands.push_back(Hand(sf::Vector2f(350.f, 500.f)));

        activeHandIndex = 0;
        state = DEALING_START;
        dealStep = 0;
        dealTimer = 0.f;
        resultMessage = "";
        dealerTimer = 0.f;

        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
    };

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Event event;

        bool isAnyAnimating = false;
        for (const auto& c : dealerHand.cards) if (c.isAnimating) isAnyAnimating = true;
        for (const auto& h : playerHands)
            for (const auto& c : h.cards) if (c.isAnimating) isAnyAnimating = true;

        sf::Vector2i mousePosI = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePosI);

        if (state == MENU) {
            playBtn.update(mousePos);
            settingsBtn.update(mousePos);
            exitBtn.update(mousePos);
        } else if (state == GAMEPLAY_OPTIONS) {
            modeClassicBtn.update(mousePos);
            modeCustomBtn.update(mousePos);
            diffNormalBtn.update(mousePos);
            diffHardBtn.update(mousePos);
            startGameBtn.update(mousePos);
            backToMenuFromOptionsBtn.update(mousePos);
        } else if (state == BETTING) {
            chip1.setEnabled(balance >= 1);
            chip5.setEnabled(balance >= 5);
            chip25.setEnabled(balance >= 25);
            chip10.setEnabled(balance >= 10);
            chip100.setEnabled(balance >= 100);
            chip500.setEnabled(balance >= 500);
            chip1000.setEnabled(balance >= 1000);
            chip5000.setEnabled(balance >= 5000);
            chip10000.setEnabled(balance >= 10000);
            chip25000.setEnabled(balance >= 25000);
            resetBetBtn.setEnabled(currentBet > 0);
            dealBtn.setEnabled(currentBet > 0);

            chip1.update(mousePos);
            chip5.update(mousePos);
            chip25.update(mousePos);
            chip10.update(mousePos);
            chip100.update(mousePos);
            chip500.update(mousePos);
            chip1000.update(mousePos);
            chip5000.update(mousePos);
            chip10000.update(mousePos);
            chip25000.update(mousePos);
            resetBetBtn.update(mousePos);
            dealBtn.update(mousePos);
            backToMenuBtn.update(mousePos);
            bailoutBtn.update(mousePos);
        } else if (state == PLAYER_TURN) {
            if (!playerHands.empty() && activeHandIndex < playerHands.size()) {
                Hand& currentHand = playerHands[activeHandIndex];
                bool canSplit = (playerHands.size() == 1 && currentHand.cards.size() == 2 &&
                                 currentHand.cards[0].value == currentHand.cards[1].value &&
                                 balance >= currentBet);

                hitBtn.setEnabled(currentHand.getTotal() <= 21 && !isAnyAnimating && !isPeekingChoiceActive);
                standBtn.setEnabled(!isAnyAnimating && !isPeekingChoiceActive);
                splitBtn.setEnabled(canSplit && !isAnyAnimating && !isPeekingChoiceActive);
                
                if (hasSkillsMode) {
                    lifelineBtn.setEnabled(currentHand.getTotal() > 21 && !lifelineUsed && balance >= 25 && !isAnyAnimating && !isPeekingChoiceActive);
                    peekBtn.setEnabled(!peekUsed && balance >= 25 && currentHand.getTotal() <= 21 && !isAnyAnimating && !isPeekingChoiceActive);
                }
            }
            hitBtn.update(mousePos);
            standBtn.update(mousePos);
            splitBtn.update(mousePos);
            
            if (hasSkillsMode) {
                if (isPeekingChoiceActive) {
                    peekDeckBtn.update(mousePos);
                    peekDealerBtn.update(mousePos);
                    cancelPeekBtn.update(mousePos);
                } else {
                    lifelineBtn.update(mousePos);
                    peekBtn.update(mousePos);
                }
            }
        } else if (state == SETTINGS) {
            bg1Btn.update(mousePos);
            bg2Btn.update(mousePos);
            tableGreenBtn.update(mousePos);
            tableBlueBtn.update(mousePos);
            tableRedBtn.update(mousePos);
            cbRedBtn.update(mousePos);
            cbBlueBtn.update(mousePos);
            cbGreenBtn.update(mousePos);
            fullscreenBtn.update(mousePos);
            resetProgressBtn.update(mousePos);
            backFromSettingsBtn.update(mousePos);
        } else if (state == GAME_OVER) {
            newRoundBtn.update(mousePos);
        }

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::Resized) {
                sf::View newView(sf::FloatRect(0.f, 0.f, 1024.f, 768.f));
                gameView = getLetterboxView(newView, event.size.width, event.size.height);
                window.setView(gameView);
            }

            bool mouseClicked = (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left);

            if (mouseClicked) {
                if (state == MENU) {
                    if (playBtn.isPressed(mousePos, mouseClicked)) { state = GAMEPLAY_OPTIONS; updateGameplayOptionsLabels(); }
                    else if (settingsBtn.isPressed(mousePos, mouseClicked)) { state = SETTINGS; updateSettingsButtonLabels(); }
                    else if (exitBtn.isPressed(mousePos, mouseClicked)) { window.close(); }
                }
                else if (state == GAMEPLAY_OPTIONS) {
                    if (modeClassicBtn.isPressed(mousePos, mouseClicked)) { hasSkillsMode = false; updateGameplayOptionsLabels(); }
                    else if (modeCustomBtn.isPressed(mousePos, mouseClicked)) { hasSkillsMode = true; updateGameplayOptionsLabels(); }
                    else if (diffNormalBtn.isPressed(mousePos, mouseClicked)) { isHardMode = false; updateGameplayOptionsLabels(); }
                    else if (diffHardBtn.isPressed(mousePos, mouseClicked)) { isHardMode = true; updateGameplayOptionsLabels(); }
                    else if (startGameBtn.isPressed(mousePos, mouseClicked)) { state = BETTING; saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen); }
                    else if (backToMenuFromOptionsBtn.isPressed(mousePos, mouseClicked)) { state = MENU; }
                }
                else if (state == BETTING) {
                    if (chip1.isPressed(mousePos, mouseClicked) && balance >= 1) { currentBet += 1; balance -= 1; handleSound.play(); }
                    else if (chip5.isPressed(mousePos, mouseClicked) && balance >= 5) { currentBet += 5; balance -= 5; handleSound.play(); }
                    else if (chip25.isPressed(mousePos, mouseClicked) && balance >= 25) { currentBet += 25; balance -= 25; handleSound.play(); }
                    else if (chip10.isPressed(mousePos, mouseClicked) && balance >= 10) { currentBet += 10; balance -= 10; handleSound.play(); }
                    else if (chip100.isPressed(mousePos, mouseClicked) && balance >= 100) { currentBet += 100; balance -= 100; handleSound.play(); }
                    else if (chip500.isPressed(mousePos, mouseClicked) && balance >= 500) { currentBet += 500; balance -= 500; handleSound.play(); }
                    else if (chip1000.isPressed(mousePos, mouseClicked) && balance >= 1000) { currentBet += 1000; balance -= 1000; handleSound.play(); }
                    else if (chip5000.isPressed(mousePos, mouseClicked) && balance >= 5000) { currentBet += 5000; balance -= 5000; handleSound.play(); }
                    else if (chip10000.isPressed(mousePos, mouseClicked) && balance >= 10000) { currentBet += 10000; balance -= 10000; handleSound.play(); }
                    else if (chip25000.isPressed(mousePos, mouseClicked) && balance >= 25000) { currentBet += 25000; balance -= 25000; handleSound.play(); }
                    else if (resetBetBtn.isPressed(mousePos, mouseClicked)) { balance += currentBet; currentBet = 0; saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen); handleSound.play(); }
                    else if (backToMenuBtn.isPressed(mousePos, mouseClicked)) { state = MENU; saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen); }
                    else if (dealBtn.isPressed(mousePos, mouseClicked) && currentBet > 0) { startRound(); }
                    else if (bailoutBtn.isPressed(mousePos, mouseClicked) && balance == 0 && currentBet == 0) { balance = 1000; saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen); handleSound.play(); }
                }
                else if (state == PLAYER_TURN && !isAnyAnimating) {
                    Hand& currentHand = playerHands[activeHandIndex];
                    bool canSplit = (playerHands.size() == 1 && currentHand.cards.size() == 2 &&
                                     currentHand.cards[0].value == currentHand.cards[1].value &&
                                     balance >= currentBet);

                    if (!isPeekingChoiceActive) {
                        if (hitBtn.isPressed(mousePos, mouseClicked) && currentHand.getTotal() <= 21) {
                            currentHand.addCard(deck.drawCard());
                            placeSound.play();
                            if (currentHand.getTotal() > 21) {
                                if (!hasSkillsMode || lifelineUsed || balance < 25){
                                    activeHandIndex++;
                                }
                            }
                        }
                        else if (standBtn.isPressed(mousePos, mouseClicked)) {
                            activeHandIndex++;
                        }
                        else if (splitBtn.isPressed(mousePos, mouseClicked) && canSplit) {
                            balance -= currentBet;
                            currentBet *= 2;
                            Card splitCard = currentHand.cards.back();
                            currentHand.cards.pop_back();

                            currentHand.startPos = sf::Vector2f(150.f, 500.f);
                            currentHand.updateCardPositions();

                            Hand secondHand(sf::Vector2f(550.f, 500.f));
                            splitCard.currentPos = currentHand.cards[0].currentPos;
                            secondHand.addCard(splitCard);
                            placeSound.play();

                            playerHands.push_back(secondHand);
                        }
                    }

                    if (hasSkillsMode) {
                        if (isPeekingChoiceActive) {
                            if (peekDeckBtn.isPressed(mousePos, mouseClicked) && balance >= 25) {
                                balance -= 25;
                                peekUsed = true;
                                deck.activatePeek();
                                isPeekingChoiceActive = false;
                                handleSound.play();
                            }
                            else if (peekDealerBtn.isPressed(mousePos, mouseClicked) && balance >= 25) {
                                balance -= 25;
                                peekUsed = true;
                                isDealerCardRevealed = true;
                                isPeekingChoiceActive = false;
                                handleSound.play();
                            }
                            else if (cancelPeekBtn.isPressed(mousePos, mouseClicked)) {
                                isPeekingChoiceActive = false;
                            }
                        } else {
                            if (lifelineBtn.isPressed(mousePos, mouseClicked) && currentHand.getTotal() > 21 && !lifelineUsed && balance >= 25) {
                                balance -= 25;
                                lifelineUsed = true;
                                Card lifelineCard = deck.drawCard();
                                lifelineCard.isSubstracting = true;
                                lifelineCard.frontSprite.setTexture(texManager.getInverted(lifelineCard.texturePath));
                                currentHand.addCard(lifelineCard);
                                placeSound.play();
                                if (currentHand.getTotal() > 21){
                                    activeHandIndex++;
                                }
                            }
                            else if (peekBtn.isPressed(mousePos, mouseClicked) && !peekUsed && balance >= 25 && currentHand.getTotal() <= 21) {
                                isPeekingChoiceActive = true;
                            }
                        }
                    }

                    if (activeHandIndex >= playerHands.size()) {
                        bool allBusted = true;
                        for (const auto& h : playerHands) if (h.getTotal() <= 21) allBusted = false;
                        state = allBusted ? GAME_OVER : DEALER_TURN;
                    }
                }

                else if (state == SETTINGS) {
                    if (bg1Btn.isPressed(mousePos, mouseClicked)) {
                        currentBgPath = "textures/Backgrounds/background_1.png";
                        updateBgTexture(currentBgPath);
                        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                        updateSettingsButtonLabels();
                    }
                    else if (bg2Btn.isPressed(mousePos, mouseClicked)) {
                        currentBgPath = "textures/Backgrounds/background_2.png";
                        updateBgTexture(currentBgPath);
                        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                        updateSettingsButtonLabels();
                    }
                    else if (tableGreenBtn.isPressed(mousePos, mouseClicked)) {
                        currentTablePath = "textures/Tables/table_green.png.png";
                        updateTableTexture(currentTablePath);
                        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                        updateSettingsButtonLabels();
                    }
                    else if (tableBlueBtn.isPressed(mousePos, mouseClicked)) {
                        currentTablePath = "textures/Tables/table_blue.png";
                        updateTableTexture(currentTablePath);
                        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                        updateSettingsButtonLabels();
                    }
                    else if (tableRedBtn.isPressed(mousePos, mouseClicked)) {
                        currentTablePath = "textures/Tables/table_red.png";
                        updateTableTexture(currentTablePath);
                        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                        updateSettingsButtonLabels();
                    }
                    else if (cbRedBtn.isPressed(mousePos, mouseClicked)) {
                        currentCbPath = "textures/cardback/cardBackRed.png";
                        deck.setCardBackPath(currentCbPath);
                        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                        updateSettingsButtonLabels();
                    }
                    else if (cbBlueBtn.isPressed(mousePos, mouseClicked)) {
                        currentCbPath = "textures/cardback/cardBackBlue.png";
                        deck.setCardBackPath(currentCbPath);
                        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                        updateSettingsButtonLabels();
                    }
                    else if (cbGreenBtn.isPressed(mousePos, mouseClicked)) {
                        currentCbPath = "textures/cardback/cardBackGreen.png";
                        deck.setCardBackPath(currentCbPath);
                        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                        updateSettingsButtonLabels();
                    }
                    else if (fullscreenBtn.isPressed(mousePos, mouseClicked)) {
                        isFullscreen = !isFullscreen;
                        toggleWindowMode(isFullscreen);
                        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                        updateSettingsButtonLabels();
                    }
                    else if (resetProgressBtn.isPressed(mousePos, mouseClicked)) {
                        balance = 1000;
                        saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                        updateSettingsButtonLabels();
                    }
                    else if (backFromSettingsBtn.isPressed(mousePos, mouseClicked)) {
                        state = MENU;
                    }
                }
                else if (state == GAME_OVER && !isAnyAnimating) {
                    if (newRoundBtn.isPressed(mousePos, mouseClicked)) {
                        currentBet = 0;
                        state = BETTING;
                    }
                }
            }
        }

        for (auto& c : dealerHand.cards) c.updateAnimation(dt);
        for (auto& h : playerHands)
            for (auto& c : h.cards) c.updateAnimation(dt);

        if (state == DEALING_START && !isAnyAnimating) {
            dealTimer += dt;
            if (dealTimer >= 0.35f) {
                dealTimer = 0.f;
                if (dealStep == 0) {
                    playerHands[0].addCard(deck.drawCard());
                    placeSound.play();
                    dealStep++;
                } else if (dealStep == 1) {
                    dealerHand.addCard(deck.drawCard());
                    placeSound.play();
                    dealStep++;
                } else if (dealStep == 2) {
                    playerHands[0].addCard(deck.drawCard());
                    placeSound.play();
                    dealStep++;
                } else if (dealStep == 3) {
                    dealerHand.addCard(deck.drawCard());
                    placeSound.play();
                    dealStep++;
                } else if (dealStep == 4) {
                    state = PLAYER_TURN;
                }
            }
        }

        if (state == DEALER_TURN && !isAnyAnimating) {
            dealerTimer += dt;
            if (dealerTimer >= 0.8f) {
                dealerTimer = 0.f;
                int dealerLimit = isHardMode ? 18 : 17;
                if (dealerHand.getTotal() < dealerLimit) {
                    dealerHand.addCard(deck.drawCard());
                    placeSound.play();
                } else {
                    state = GAME_OVER;
                }
            }
        }

        if (state == GAME_OVER && resultMessage.empty()) {
            int dTotal = dealerHand.getTotal();
            float betPerHand = (playerHands.size() > 1) ? (currentBet / 2.f) : (float)currentBet;

            for (size_t i = 0; i < playerHands.size(); ++i) {
                int pTotal = playerHands[i].getTotal();
                bool isBlackjack = (playerHands.size() == 1 && playerHands[i].cards.size() == 2 && pTotal == 21);
                std::string prefix = playerHands.size() > 1 ? ("Hand " + std::to_string(i + 1) + ": ") : "";

                if (pTotal > 21) {
                    resultMessage += prefix + "Busted.\n";
                } else if (isBlackjack && dTotal != 21) {
                    resultMessage += prefix + "BLACKJACK!\n";
                    balance += static_cast<int>(betPerHand * (isHardMode ? 3.0f : 2.5f));
                } else if (dTotal > 21) {
                    resultMessage += prefix + "Dealer busted! You win!\n";
                    balance += static_cast<int>(betPerHand * (isHardMode ? 2.5f : 2.0f));
                } else if (pTotal > dTotal) {
                    resultMessage += prefix + "You win!\n";
                    balance += static_cast<int>(betPerHand * (isHardMode ? 2.5f : 2.0f));
                } else if (dTotal > pTotal) {
                    resultMessage += prefix + "You lose.\n";
                } else {
                    resultMessage += prefix + "Push.\n";
                    balance += static_cast<int>(betPerHand);
                }
            }
            saveGame(balance, currentBgPath, currentTablePath, currentCbPath, isFullscreen);
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(tableSprite);

        std::string ecoStr = "Balance: $" + std::to_string(balance) + "\nBet: $" + std::to_string(currentBet) + "\n\n";

        if (state == MENU) {
            uiText.setString("BLACKJACK PP");
            sf::FloatRect textRect = uiText.getLocalBounds();
            uiText.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
            uiText.setPosition(512.f, 220.f);
            window.draw(uiText);

            playBtn.draw(window);
            settingsBtn.draw(window);
            exitBtn.draw(window);
        }
        else if (state == GAMEPLAY_OPTIONS) {
            uiText.setString("GAMEPLAY OPTIONS");
            sf::FloatRect textRect = uiText.getLocalBounds();
            uiText.setOrigin(textRect.left + textRect.width / 2.f, 0.f);
            uiText.setPosition(512.f, 100.f);
            window.draw(uiText);

            sf::Text modeLabel("SELECT GAMEPLAY MODE:", font, 18);
            modeLabel.setFillColor(sf::Color::White);
            sf::FloatRect modeRect = modeLabel.getLocalBounds();
            modeLabel.setOrigin(modeRect.left + modeRect.width / 2.f, 0.f);
            modeLabel.setPosition(512.f, 170.f);
            window.draw(modeLabel);

            modeClassicBtn.draw(window);
            modeCustomBtn.draw(window);

            sf::Text diffLabel("SELECT DIFFICULTY:", font, 18);
            diffLabel.setFillColor(sf::Color::White);
            sf::FloatRect diffRect = diffLabel.getLocalBounds();
            diffLabel.setOrigin(diffRect.left + diffRect.width / 2.f, 0.f);
            diffLabel.setPosition(512.f, 290.f);
            window.draw(diffLabel);

            diffNormalBtn.draw(window);
            diffHardBtn.draw(window);

            startGameBtn.draw(window);
            backToMenuFromOptionsBtn.draw(window);
        }
        else if (state == BETTING) {
            uiText.setString(ecoStr + "PLACE YOUR BET:");
            sf::FloatRect textRect = uiText.getLocalBounds();
            uiText.setOrigin(textRect.left + textRect.width / 2.f, 0.f);
            uiText.setPosition(512.f, 150.f);
            window.draw(uiText);

            chip1.draw(window);
            chip5.draw(window);
            chip25.draw(window);
            chip10.draw(window);
            chip100.draw(window);
            chip500.draw(window);
            chip1000.draw(window);
            chip5000.draw(window);
            chip10000.draw(window);
            chip25000.draw(window);
            resetBetBtn.draw(window);
            dealBtn.draw(window);
            backToMenuBtn.draw(window);
            if (balance == 0 && currentBet == 0) {
                bailoutBtn.draw(window);
            }
        }
        else if (state == SETTINGS) {
            uiText.setString("GAME SETTINGS");
            sf::FloatRect textRect = uiText.getLocalBounds();
            uiText.setOrigin(textRect.left + textRect.width / 2.f, 0.f);
            uiText.setPosition(512.f, 80.f);
            window.draw(uiText);

            sf::Text bgLabel("SELECT BACKGROUND:", font, 18);
            bgLabel.setFillColor(sf::Color::White);
            sf::FloatRect bgRect = bgLabel.getLocalBounds();
            bgLabel.setOrigin(bgRect.left + bgRect.width / 2.f, 0.f);
            bgLabel.setPosition(512.f, 130.f);
            window.draw(bgLabel);

            bg1Btn.draw(window);
            bg2Btn.draw(window);

            sf::Text tableLabel("SELECT TABLE:", font, 18);
            tableLabel.setFillColor(sf::Color::White);
            sf::FloatRect tableRect = tableLabel.getLocalBounds();
            tableLabel.setOrigin(tableRect.left + tableRect.width / 2.f, 0.f);
            tableLabel.setPosition(512.f, 220.f);
            window.draw(tableLabel);

            tableGreenBtn.draw(window);
            tableBlueBtn.draw(window);
            tableRedBtn.draw(window);

            sf::Text cbLabel("SELECT CARD BACK:", font, 18);
            cbLabel.setFillColor(sf::Color::White);
            sf::FloatRect cbRect = cbLabel.getLocalBounds();
            cbLabel.setOrigin(cbRect.left + cbRect.width / 2.f, 0.f);
            cbLabel.setPosition(512.f, 310.f);
            window.draw(cbLabel);

            cbRedBtn.draw(window);
            cbBlueBtn.draw(window);
            cbGreenBtn.draw(window);

            fullscreenBtn.draw(window);
            resetProgressBtn.draw(window);
            backFromSettingsBtn.draw(window);

            sf::Sprite bgPreviewSprite;
            sf::Texture& bgTex = texManager.get(currentBgPath);
            bgPreviewSprite.setTexture(bgTex, true);
            bgPreviewSprite.setPosition(70.f, 230.f);
            bgPreviewSprite.setScale(140.f / bgTex.getSize().x, 105.f / bgTex.getSize().y);
            window.draw(bgPreviewSprite);

            sf::Sprite tablePreviewSprite;
            sf::Texture& tableTex = texManager.get(currentTablePath);
            tablePreviewSprite.setTexture(tableTex, true);
            tablePreviewSprite.setPosition(70.f, 230.f);
            tablePreviewSprite.setScale(140.f / tableTex.getSize().x, 105.f / tableTex.getSize().y);
            window.draw(tablePreviewSprite);

            sf::RectangleShape tableBorder(sf::Vector2f(140.f, 105.f));
            tableBorder.setPosition(70.f, 230.f);
            tableBorder.setFillColor(sf::Color::Transparent);
            tableBorder.setOutlineColor(sf::Color(46, 204, 113));
            tableBorder.setOutlineThickness(2.f);
            window.draw(tableBorder);

            sf::Text tablePreviewLabel("TABLE PREVIEW", font, 16);
            tablePreviewLabel.setFillColor(sf::Color::White);
            sf::FloatRect tpRect = tablePreviewLabel.getLocalBounds();
            tablePreviewLabel.setOrigin(tpRect.left + tpRect.width / 2.f, 0.f);
            tablePreviewLabel.setPosition(140.f, 200.f);
            window.draw(tablePreviewLabel);

            sf::Sprite cbPreviewSprite;
            sf::Texture& cbTex = texManager.get(currentCbPath);
            cbPreviewSprite.setTexture(cbTex, true);
            float cbScale = 140.f / cbTex.getSize().y;
            cbPreviewSprite.setScale(cbScale, cbScale);
            float cbWidth = cbTex.getSize().x * cbScale;
            cbPreviewSprite.setPosition(884.f - cbWidth / 2.f, 230.f);
            window.draw(cbPreviewSprite);

            sf::RectangleShape cbBorder(sf::Vector2f(cbWidth, 140.f));
            cbBorder.setPosition(884.f - cbWidth / 2.f, 230.f);
            cbBorder.setFillColor(sf::Color::Transparent);
            cbBorder.setOutlineColor(sf::Color(46, 204, 113));
            cbBorder.setOutlineThickness(2.f);
            window.draw(cbBorder);

            sf::Text cbPreviewLabel("CARD BACK PREVIEW", font, 16);
            cbPreviewLabel.setFillColor(sf::Color::White);
            sf::FloatRect cbpRect = cbPreviewLabel.getLocalBounds();
            cbPreviewLabel.setOrigin(cbpRect.left + cbpRect.width / 2.f, 0.f);
            cbPreviewLabel.setPosition(884.f, 200.f);
            window.draw(cbPreviewLabel);
        }
        else {
            if (hasSkillsMode) {
                sf::RectangleShape skillsFrame(sf::Vector2f(174.f, 560.f));
                skillsFrame.setPosition(830.f, 120.f);
                skillsFrame.setFillColor(sf::Color(0, 0, 0, 100));
                skillsFrame.setOutlineColor(sf::Color(46, 204, 113));
                skillsFrame.setOutlineThickness(2.f);
                window.draw(skillsFrame);

                sf::Text skillsTitle("SKILLS", font, 18);
                skillsTitle.setFillColor(sf::Color(46, 204, 113));
                sf::FloatRect stRect = skillsTitle.getLocalBounds();
                skillsTitle.setOrigin(stRect.left + stRect.width / 2.f, 0.f);
                skillsTitle.setPosition(917.f, 130.f);
                window.draw(skillsTitle);
            }

            deck.draw(window);
            dealerHand.draw(window, (state == PLAYER_TURN || state == DEALING_START) && !isDealerCardRevealed, isDealerCardRevealed && (state == PLAYER_TURN || state == DEALING_START));
            for (auto& hand : playerHands) hand.draw(window);

            std::string statusStr = ecoStr + "Dealer:\nTotal: ";
            if (state != PLAYER_TURN && state != DEALING_START || isDealerCardRevealed) statusStr += std::to_string(dealerHand.getTotal()) + "\n";
            else statusStr += "?\n";

            statusStr += "\nYou:\n";
            for (size_t i = 0; i < playerHands.size(); ++i) {
                if (playerHands.size() > 1) statusStr += "Hand " + std::to_string(i + 1) + " ";
                if ((state == PLAYER_TURN || state == DEALING_START) && i == activeHandIndex) statusStr += "(Active) ";
                statusStr += "Total: " + std::to_string(playerHands[i].getTotal()) + "\n";
            }

            statusStr += "\n";

            if (state == PLAYER_TURN) {
                if (isAnyAnimating) {
                    statusStr += "Dealing...";
                } else {
                    if (playerHands[activeHandIndex].getTotal() > 21){
                        statusStr += "BUSTED!\n";
                    }else{
                         statusStr += "Choose action...";
                    }
                    if (deck.getIsPeeking()) {
                        statusStr += "\nNext card: [ "
                                     + deck.getPeekDescription()
                                     + " ]\n";
                    }
                }
            } else if (state == GAME_OVER) {
                statusStr += "RESULT:\n" + resultMessage;
            } else if (state == DEALER_TURN) {
                statusStr += "Dealer's Turn...";
            } else if (state == DEALING_START) {
                statusStr += "Dealing cards...";
            }

            uiText.setOrigin(0.f, 0.f);
            uiText.setString(statusStr);
            uiText.setPosition(30.f, 150.f);
            window.draw(uiText);

            if (state == PLAYER_TURN) {
                hitBtn.draw(window);
                standBtn.draw(window);
                splitBtn.draw(window);
                if (hasSkillsMode) {
                    if (isPeekingChoiceActive) {
                        peekDeckBtn.draw(window);
                        peekDealerBtn.draw(window);
                        cancelPeekBtn.draw(window);
                    } else {
                        lifelineBtn.draw(window);
                        peekBtn.draw(window);
                    }
                }
            } else if (state == GAME_OVER) {
                newRoundBtn.draw(window);
            }
        }

        window.display();
    }

    return 0;
}
