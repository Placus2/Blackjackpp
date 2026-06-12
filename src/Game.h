#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <string>

#include "TextureManager.h"
#include "SoundManager.h"
#include "Button.h"
#include "Deck.h"
#include "Hand.h"
#include "SaveManager.h"

// Enum GameState is defined in GameObject.h

class Game {
public:
    Game();
    void run();

private:
    void initUI();
    void initChips();
    void handleEvents();
    void update(float deltaTime);
    void render();
    void startRound();
    void resetRoundState();
    
    // UI Helpers
    void updateAllStaticLabels();
    void rebuildGameObjects();
    void rebuildProfileButtons();
    void updateSettingsButtonLabels();
    void updateGameplayOptionsLabels();
    void toggleWindowMode(bool fullscreen);
    void updateBgTexture(const std::string& path);
    void updateTableTexture(const std::string& path);
    void updateLogoTexture();
    void evaluteWinnersAndPay();

    sf::RenderWindow window;
    sf::View gameView;
    GameState state;

    std::string currentPlayerName;
    std::string typedName;
    int balance;

    std::string currentBgPath;
    std::string currentTablePath;
    std::string currentCbPath;
    bool isFullscreen;
    bool isPolish;

    sf::Font font;
    sf::Font tempFont;
    sf::Text uiText;

    TextureManager texManager;
    SoundManager soundManager;

    sf::Texture bgTexture;
    sf::Texture tableTexture;
    sf::Texture logoTexture;
    sf::Sprite bgSprite;
    sf::Sprite tableSprite;
    sf::Sprite logoSprite;

    std::vector<std::shared_ptr<GameObject>> gameObjects;

    std::shared_ptr<Deck> deckPtr;
    std::shared_ptr<Hand> dealerHandPtr;
    std::vector<std::shared_ptr<Hand>> playerHands;

    int currentBet;
    int dealStep;
    float dealTimer;
    float dealerTimer;
    size_t activeHandIndex;
    std::string resultMessage;

    bool lifelineUsed;
    bool peekUsed;
    bool swapUsed;
    bool isPeekingChoiceActive;
    bool isDealerCardRevealed;
    bool hasSkillsMode;
    bool isHardMode;

    // Buttons
    std::shared_ptr<Button> createProfileBtn;
    std::shared_ptr<Button> backToMenuFromProfileBtn;
    std::shared_ptr<Button> backFromProfileCreateBtn;
    std::vector<std::shared_ptr<Button>> profileBtns;
    std::vector<std::shared_ptr<Button>> deleteProfileBtns;

    std::shared_ptr<Button> playBtn;
    std::shared_ptr<Button> settingsBtn;
    std::shared_ptr<Button> leaderboardBtn;
    std::shared_ptr<Button> exitBtn;

    std::shared_ptr<Button> modeClassicBtn;
    std::shared_ptr<Button> modeCustomBtn;
    std::shared_ptr<Button> diffNormalBtn;
    std::shared_ptr<Button> diffHardBtn;
    std::shared_ptr<Button> startGameBtn;
    std::shared_ptr<Button> backToMenuFromOptionsBtn;

    std::shared_ptr<Button> resetBetBtn;
    std::shared_ptr<Button> dealBtn;
    std::shared_ptr<Button> backToMenuBtn;
    std::shared_ptr<Button> bailoutBtn;

    std::shared_ptr<Button> hitBtn;
    std::shared_ptr<Button> standBtn;
    std::shared_ptr<Button> splitBtn;
    std::shared_ptr<Button> lifelineBtn;
    std::shared_ptr<Button> peekBtn;
    std::shared_ptr<Button> swapBtn;

    std::shared_ptr<Button> peekDeckBtn;
    std::shared_ptr<Button> peekDealerBtn;
    std::shared_ptr<Button> cancelPeekBtn;
    std::shared_ptr<Button> newRoundBtn;

    std::shared_ptr<Button> bg1Btn;
    std::shared_ptr<Button> bg2Btn;
    std::shared_ptr<Button> tableGreenBtn;
    std::shared_ptr<Button> tableBlueBtn;
    std::shared_ptr<Button> tableRedBtn;
    std::shared_ptr<Button> cbRedBtn;
    std::shared_ptr<Button> cbBlueBtn;
    std::shared_ptr<Button> cbGreenBtn;
    std::shared_ptr<Button> fullscreenBtn;
    std::shared_ptr<Button> langEnBtn;
    std::shared_ptr<Button> langPlBtn;
    std::shared_ptr<Button> backFromSettingsBtn;
    std::shared_ptr<Button> leaderboardBackBtn;

    // Chips
    std::shared_ptr<ChipButton> chip1;
    std::shared_ptr<ChipButton> chip5;
    std::shared_ptr<ChipButton> chip10;
    std::shared_ptr<ChipButton> chip25;
    std::shared_ptr<ChipButton> chip100;
    std::shared_ptr<ChipButton> chip500;
    std::shared_ptr<ChipButton> chip1000;
    std::shared_ptr<ChipButton> chip5000;
    std::shared_ptr<ChipButton> chip10000;
    std::shared_ptr<ChipButton> chip25000;
};

// Funkcje z main.cpp
std::string getPlayerName();
sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight);
std::string t(const std::string& en, const std::string& pl, bool isPolish);

#endif // GAME_H
