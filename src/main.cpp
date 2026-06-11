#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <memory>
#include <algorithm>
#include <cstdlib>

#include "TextureManager.h"
#include "Card.h"
#include "Hand.h"
#include "Deck.h"
#include "Button.h"

// TABELA WYNIKÓW
struct Score {
    std::string name;
    int value;
};



// Zwraca nazwe aktualnego uzytkownika systemu
std::string getPlayerName() {
    const char* user = std::getenv("USER");
    if (!user) user = std::getenv("USERNAME");
    return user ? std::string(user) : "Player";
}

// Zapisuje najlepsze wyniki graczy do pliku tekstowego
void saveHighscores(const std::vector<Score>& scores) {
    std::filesystem::create_directories("Save");
    std::ofstream file("Save/highscores.txt");
    if (file.is_open()) {
        for (const auto& s : scores) {
            file << s.name << "\n" << s.value << "\n";
        }
        file.close();
    }
}

// Wczytuje najlepsze wyniki graczy z pliku tekstowego
std::vector<Score> loadHighscores() {
    std::vector<Score> scores;
    std::ifstream file("Save/highscores.txt");
    if (file.is_open()) {
        std::string nameLine, valueLine;
        while (std::getline(file, nameLine) && std::getline(file, valueLine)) {
            if (!nameLine.empty() && nameLine.back() == '\r') nameLine.pop_back();
            if (!valueLine.empty() && valueLine.back() == '\r') valueLine.pop_back();
            try {
                Score s;
                s.name = nameLine;
                s.value = std::stoi(valueLine);
                scores.push_back(s);
            } catch (...) {}
        }
        file.close();
    }
    return scores;
}

// Aktualizuje wynik gracza na liscie najlepszych wynikow i zapisuje dane
void updateLeaderboard(const std::string& playerName, int newBalance) {
    auto scores = loadHighscores();
    bool found = false;
    for (auto& s : scores) {
        if (s.name == playerName) {
            s.value = newBalance;
            found = true;
            break;
        }
    }
    if (!found) {
        scores.push_back({playerName, newBalance});
    }
    saveHighscores(scores);
}

// Dostosowuje widok okna gry, aby zachowac proporcje ekranu (Letterbox)
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

// Zapisuje globalne ustawienia gry (tlo, stol, rewers, pelny ekran) do pliku
void saveGlobalSettings(const std::string& bgPath, const std::string& tablePath, const std::string& cbPath, bool isFullscreen) {
    std::filesystem::create_directories("Save");
    std::ofstream file("Save/global_settings.txt");
    if (file.is_open()) {
        file << bgPath << "\n" << tablePath << "\n" << cbPath << "\n" << (isFullscreen ? 1 : 0) << "\n";
    }
}

// Wczytuje globalne ustawienia gry z pliku, lub ustawia domyslne
void loadGlobalSettings(std::string& bgPath, std::string& tablePath, std::string& cbPath, bool& isFullscreen) {
    std::ifstream file("Save/global_settings.txt");
    bool success = false;
    if (file.is_open()) {
        std::vector<std::string> lines; std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            lines.push_back(line);
        }
        if (lines.size() >= 4) {
            try {
                bgPath = lines[0]; tablePath = lines[1]; cbPath = lines[2]; isFullscreen = (std::stoi(lines[3]) != 0);
                success = true;
            } catch (...) { success = false; }
        }
    }
    if (!success) {
        bgPath = "textures/Backgrounds/background_1.png";
        tablePath = "textures/Tables/table_green.png.png";
        cbPath = "textures/cardback/cardBackRed.png";
        isFullscreen = false;
        saveGlobalSettings(bgPath, tablePath, cbPath, isFullscreen);
    }
}

// Zapisuje aktualne saldo punktow/pieniedzy gracza
void saveBalance(const std::string& playerName, int balance) {
    if (playerName.empty()) return;
    updateLeaderboard(playerName, balance);
}

// Wczytuje saldo punktow/pieniedzy gracza na podstawie jego nazwy
void loadBalance(const std::string& playerName, int& balance) {
    auto scores = loadHighscores();
    for (const auto& s : scores) {
        if (s.name == playerName) {
            balance = s.value;
            break;
        }
    }
}

// Glowna funkcja programu, zarzadza petla gry i oknem aplikacji
int main() {
    std::string currentPlayerName = "";
    std::string typedName = "";
    int balance = 1000;
    std::string currentBgPath = "textures/Backgrounds/background_1.png";
    std::string currentTablePath = "textures/Tables/table_green.png.png";
    std::string currentCbPath = "textures/cardback/cardBackRed.png";
    bool isFullscreen = false;

    loadGlobalSettings(currentBgPath, currentTablePath, currentCbPath, isFullscreen);

    // TWORZENIE NOWEGO PROFILU GRACZA
    sf::Font tempFont;
    tempFont.loadFromFile("textures/arial.ttf");
    auto createProfileBtn = std::make_shared<Button>(362.f, 600.f, 300.f, 50.f, "NEW PROFILE", tempFont, 20);
    createProfileBtn->setDrawState(PROFILE_SELECT);
    
    auto backToMenuFromProfileBtn = std::make_shared<Button>(362.f, 660.f, 300.f, 50.f, "BACK TO MENU", tempFont, 20);
    backToMenuFromProfileBtn->setDrawState(PROFILE_SELECT);

    auto backFromProfileCreateBtn = std::make_shared<Button>(362.f, 660.f, 300.f, 50.f, "BACK", tempFont, 20);
    backFromProfileCreateBtn->setDrawState(PROFILE_CREATE);
    
    std::vector<std::shared_ptr<Button>> profileBtns;
    std::vector<std::shared_ptr<Button>> deleteProfileBtns;
    auto rebuildProfileButtons = [&]() {
        profileBtns.clear();
        deleteProfileBtns.clear();
        auto scores = loadHighscores();
        float startY = 150.f;
        for (size_t i = 0; i < scores.size() && i < 5; ++i) { // Pokazuje maks 5 profili
            auto btn = std::make_shared<Button>(362.f, startY + i * 70.f, 300.f, 50.f, scores[i].name, tempFont, 20);
            btn->setDrawState(PROFILE_SELECT);
            profileBtns.push_back(btn);

            auto delBtn = std::make_shared<Button>(670.f, startY + i * 70.f, 50.f, 50.f, "X", tempFont, 20);
            delBtn->setDrawState(PROFILE_SELECT);
            deleteProfileBtns.push_back(delBtn);
        }
    };
    rebuildProfileButtons();;
    // WCZYTYWANIE DZWIEKOW
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
    // RENDEROWANIE OKNA
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
    // WCZYTANIE CZCIONKI
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

    sf::Sprite logoSprite;
    auto updateLogoTexture = [&]() {
        sf::Texture& logoTex = texManager.get("textures/logo.png");
        logoSprite.setTexture(logoTex, true);
        logoSprite.setOrigin(logoTex.getSize().x / 2.f, logoTex.getSize().y / 2.f);
        // Scale logo based on the original screenshot aspect ratio (587px width window)
        float scaleVal = 1024.f / 587.f;
        logoSprite.setScale(scaleVal, scaleVal);
        logoSprite.setPosition(512.f, 175.f);
    };
    updateLogoTexture();

    // INTELIGENTNE WSKAZNIKI
    auto deckPtr = std::make_shared<Deck>();
    deckPtr->setCardBackPath(currentCbPath);

    // PRZYCISKI MENU
    auto playBtn = std::make_shared<Button>(362.f, 260.f, 300.f, 50.f, "PLAY", font, 20);
    playBtn->setDrawState(MENU);
    auto settingsBtn = std::make_shared<Button>(362.f, 330.f, 300.f, 50.f, "SETTINGS", font, 20);
    settingsBtn->setDrawState(MENU);
    auto leaderboardBtn = std::make_shared<Button>(362.f, 400.f, 300.f, 50.f, "LEADERBOARD", font, 20);
    leaderboardBtn->setDrawState(MENU);
    auto exitBtn = std::make_shared<Button>(362.f, 470.f, 300.f, 50.f, "EXIT GAME", font, 20);
    exitBtn->setDrawState(MENU);

    // PRZYCISKI OPCJI ROZGRYWKI
    auto modeClassicBtn = std::make_shared<Button>(272.f, 210.f, 230.f, 45.f, "Classic", font, 18);
    modeClassicBtn->setDrawState(GAMEPLAY_OPTIONS);
    auto modeCustomBtn = std::make_shared<Button>(522.f, 210.f, 230.f, 45.f, "Custom", font, 18);
    modeCustomBtn->setDrawState(GAMEPLAY_OPTIONS);
    auto diffNormalBtn = std::make_shared<Button>(272.f, 330.f, 230.f, 45.f, "Normal", font, 18);
    diffNormalBtn->setDrawState(GAMEPLAY_OPTIONS);
    auto diffHardBtn = std::make_shared<Button>(522.f, 330.f, 230.f, 45.f, "Hard", font, 18);
    diffHardBtn->setDrawState(GAMEPLAY_OPTIONS);
    auto startGameBtn = std::make_shared<Button>(362.f, 460.f, 300.f, 50.f, "START GAME", font, 20);
    startGameBtn->setDrawState(GAMEPLAY_OPTIONS);
    auto backToMenuFromOptionsBtn = std::make_shared<Button>(362.f, 530.f, 300.f, 50.f, "BACK", font, 20);
    backToMenuFromOptionsBtn->setDrawState(GAMEPLAY_OPTIONS);

    sf::Texture& chipsTex = texManager.get("textures/chips/Chips.png");

    // ZETONY W CZASIE OBSTAWIANIA
    auto chip1 = std::make_shared<ChipButton>(312.f, 260.f, 1, "1$", font, chipsTex, sf::IntRect(0, 0, 64, 72));
    auto chip5 = std::make_shared<ChipButton>(396.f, 260.f, 5, "5$", font, chipsTex, sf::IntRect(64, 0, 64, 72));
    auto chip25 = std::make_shared<ChipButton>(480.f, 260.f, 25, "25$", font, chipsTex, sf::IntRect(128, 0, 64, 72));
    auto chip10 = std::make_shared<ChipButton>(564.f, 260.f, 10, "10$", font, chipsTex, sf::IntRect(192, 0, 64, 72));
    auto chip100 = std::make_shared<ChipButton>(648.f, 260.f, 100, "100$", font, chipsTex, sf::IntRect(256, 0, 64, 72));

    auto chip500 = std::make_shared<ChipButton>(312.f, 342.f, 500, "500$", font, chipsTex, sf::IntRect(0, 72, 64, 72));
    auto chip1000 = std::make_shared<ChipButton>(396.f, 342.f, 1000, "1k$", font, chipsTex, sf::IntRect(64, 72, 64, 72));
    auto chip5000 = std::make_shared<ChipButton>(480.f, 342.f, 5000, "5k$", font, chipsTex, sf::IntRect(128, 72, 64, 72));
    auto chip10000 = std::make_shared<ChipButton>(564.f, 342.f, 10000, "10k$", font, chipsTex, sf::IntRect(192, 72, 64, 72));
    auto chip25000 = std::make_shared<ChipButton>(648.f, 342.f, 25000, "25k$", font, chipsTex, sf::IntRect(256, 72, 64, 72));

    auto resetBetBtn = std::make_shared<Button>(242.f, 440.f, 160.f, 45.f, "Reset Bet", font, 18);
    resetBetBtn->setDrawState(BETTING);
    auto dealBtn = std::make_shared<Button>(422.f, 440.f, 180.f, 45.f, "Deal", font, 18);
    dealBtn->setDrawState(BETTING);
    auto backToMenuBtn = std::make_shared<Button>(622.f, 440.f, 160.f, 45.f, "Back", font, 18);
    backToMenuBtn->setDrawState(BETTING);
    auto bailoutBtn = std::make_shared<Button>(412.f, 500.f, 200.f, 45.f, "Recover $1000", font, 18);
    bailoutBtn->setDrawState(BETTING);

    // PRZYCISKI W TRAKCIE RUNDY GRACZA
    auto hitBtn = std::make_shared<Button>(132.f, 690.f, 120.f, 45.f, "Hit", font, 18);
    hitBtn->setDrawState(PLAYER_TURN);
    auto standBtn = std::make_shared<Button>(272.f, 690.f, 120.f, 45.f, "Stand", font, 18);
    standBtn->setDrawState(PLAYER_TURN);
    auto splitBtn = std::make_shared<Button>(412.f, 690.f, 120.f, 45.f, "Split", font, 18);
    splitBtn->setDrawState(PLAYER_TURN);

    // PRZYCISKI UMIEJETNOSCI
    auto lifelineBtn = std::make_shared<Button>(842.f, 170.f, 150.f, 45.f, "Minus card -25$", font, 14);
    lifelineBtn->setDrawState(PLAYER_TURN);
    auto peekBtn = std::make_shared<Button>(842.f, 230.f, 150.f, 45.f, "Sneaky peeky -25$", font, 14);
    peekBtn->setDrawState(PLAYER_TURN);
    // Inicjalizacja przycisku nowej umiejetnosci specjalnej "Zamiana kart"
    auto swapBtn = std::make_shared<Button>(842.f, 290.f, 150.f, 45.f, "Zamiana kart -50$", font, 14);
    swapBtn->setDrawState(PLAYER_TURN);

    auto peekDeckBtn = std::make_shared<Button>(842.f, 230.f, 150.f, 45.f, "Peek Deck", font, 16);
    peekDeckBtn->setDrawState(PLAYER_TURN);
    auto peekDealerBtn = std::make_shared<Button>(842.f, 290.f, 150.f, 45.f, "Peek Dealer", font, 16);
    peekDealerBtn->setDrawState(PLAYER_TURN);
    auto cancelPeekBtn = std::make_shared<Button>(842.f, 350.f, 150.f, 45.f, "Cancel", font, 16);
    cancelPeekBtn->setDrawState(PLAYER_TURN);

    auto newRoundBtn = std::make_shared<Button>(387.f, 600.f, 250.f, 50.f, "New Round", font, 20);
    newRoundBtn->setDrawState(GAME_OVER);

    // PRZYCISKI USTAWIEN
    auto bg1Btn = std::make_shared<Button>(350.f, 160.f, 150.f, 45.f, "[ ] Bg 1", font, 18);
    bg1Btn->setDrawState(SETTINGS);
    auto bg2Btn = std::make_shared<Button>(524.f, 160.f, 150.f, 45.f, "[ ] Bg 2", font, 18);
    bg2Btn->setDrawState(SETTINGS);

    auto tableGreenBtn = std::make_shared<Button>(272.f, 250.f, 150.f, 45.f, "[ ] Green", font, 18);
    tableGreenBtn->setDrawState(SETTINGS);
    auto tableBlueBtn = std::make_shared<Button>(442.f, 250.f, 150.f, 45.f, "[ ] Blue", font, 18);
    tableBlueBtn->setDrawState(SETTINGS);
    auto tableRedBtn = std::make_shared<Button>(612.f, 250.f, 150.f, 45.f, "[ ] Red", font, 18);
    tableRedBtn->setDrawState(SETTINGS);

    auto cbRedBtn = std::make_shared<Button>(272.f, 340.f, 150.f, 45.f, "[ ] Red", font, 18);
    cbRedBtn->setDrawState(SETTINGS);
    auto cbBlueBtn = std::make_shared<Button>(442.f, 340.f, 150.f, 45.f, "[ ] Blue", font, 18);
    cbBlueBtn->setDrawState(SETTINGS);
    auto cbGreenBtn = std::make_shared<Button>(612.f, 340.f, 150.f, 45.f, "[ ] Green", font, 18);
    cbGreenBtn->setDrawState(SETTINGS);

    auto fullscreenBtn = std::make_shared<Button>(362.f, 430.f, 300.f, 50.f, "[ ] Fullscreen", font, 18);
    fullscreenBtn->setDrawState(SETTINGS);
    // Usunięto resetProgressBtn
    auto backFromSettingsBtn = std::make_shared<Button>(362.f, 570.f, 300.f, 50.f, "Back to Menu", font, 18);
    backFromSettingsBtn->setDrawState(SETTINGS);

    // PRZYCISK POWROTU Z TABLICY WYNIKOW
    auto leaderboardBackBtn = std::make_shared<Button>(362.f, 570.f, 300.f, 50.f, "BACK", font, 20);
    leaderboardBackBtn->setDrawState(LEADERBOARD);

    auto updateSettingsButtonLabels = [&]() {
        if (currentBgPath == "textures/Backgrounds/background_1.png") {
            bg1Btn->setText("[x] Bg 1");
            bg2Btn->setText("[ ] Bg 2");
        } else {
            bg1Btn->setText("[ ] Bg 1");
            bg2Btn->setText("[x] Bg 2");
        }

        if (currentTablePath == "textures/Tables/table_green.png.png") {
            tableGreenBtn->setText("[x] Green");
            tableBlueBtn->setText("[ ] Blue");
            tableRedBtn->setText("[ ] Red");
        } else if (currentTablePath == "textures/Tables/table_blue.png") {
            tableGreenBtn->setText("[ ] Green");
            tableBlueBtn->setText("[x] Blue");
            tableRedBtn->setText("[ ] Red");
        } else {
            tableGreenBtn->setText("[ ] Green");
            tableBlueBtn->setText("[ ] Blue");
            tableRedBtn->setText("[x] Red");
        }

        if (currentCbPath == "textures/cardback/cardBackRed.png") {
            cbRedBtn->setText("[x] Red");
            cbBlueBtn->setText("[ ] Blue");
            cbGreenBtn->setText("[ ] Green");
        } else if (currentCbPath == "textures/cardback/cardBackBlue.png") {
            cbRedBtn->setText("[ ] Red");
            cbBlueBtn->setText("[x] Blue");
            cbGreenBtn->setText("[ ] Green");
        } else {
            cbRedBtn->setText("[ ] Red");
            cbBlueBtn->setText("[ ] Blue");
            cbGreenBtn->setText("[x] Green");
        }

        if (isFullscreen) {
            fullscreenBtn->setText("[x] Fullscreen");
        } else {
            fullscreenBtn->setText("[ ] Fullscreen");
        }
    };
    updateSettingsButtonLabels();

    // TALIA KRUPIERA
    auto dealerHandPtr = std::make_shared<Hand>(sf::Vector2f(350.f, 150.f));
    std::vector<std::shared_ptr<Hand>> playerHands;

    GameState state = MENU;
    std::string resultMessage = "";
    bool isHardMode = false;
    bool hasSkillsMode = true;

    int currentBet = 0;
    int activeHandIndex = 0;
    bool lifelineUsed = false;
    bool peekUsed = false;
    // Zmienna stanu okreslajaca, czy umiejetnosc zamiany kart zostala uzyta w danej rundzie
    bool swapUsed = false;
    bool isPeekingChoiceActive = false;
    bool isDealerCardRevealed = false;

    auto updateGameplayOptionsLabels = [&]() {
        if (hasSkillsMode) {
            modeClassicBtn->setText("[ ] Classic");
            modeCustomBtn->setText("[x] Custom");
        } else {
            modeClassicBtn->setText("[x] Classic");
            modeCustomBtn->setText("[ ] Custom");
        }

        if (isHardMode) {
            diffNormalBtn->setText("[ ] Normal");
            diffHardBtn->setText("[x] Hard");
        } else {
            diffNormalBtn->setText("[x] Normal");
            diffHardBtn->setText("[ ] Hard");
        }
    };
    updateGameplayOptionsLabels();

    std::vector<std::shared_ptr<GameObject>> gameObjects;

    auto rebuildGameObjects = [&]() {
        gameObjects.clear();

        for (const auto& btn : profileBtns) gameObjects.push_back(btn);
        for (const auto& btn : deleteProfileBtns) gameObjects.push_back(btn);
        gameObjects.push_back(createProfileBtn);
        gameObjects.push_back(backToMenuFromProfileBtn);
        gameObjects.push_back(backFromProfileCreateBtn);
        
        gameObjects.push_back(deckPtr);
        gameObjects.push_back(dealerHandPtr);
        for (auto& h : playerHands) {
            gameObjects.push_back(h);
        }

        gameObjects.push_back(playBtn);
        gameObjects.push_back(settingsBtn);
        gameObjects.push_back(leaderboardBtn);
        gameObjects.push_back(exitBtn);

        gameObjects.push_back(modeClassicBtn);
        gameObjects.push_back(modeCustomBtn);
        gameObjects.push_back(diffNormalBtn);
        gameObjects.push_back(diffHardBtn);
        gameObjects.push_back(startGameBtn);
        gameObjects.push_back(backToMenuFromOptionsBtn);

        gameObjects.push_back(chip1);
        gameObjects.push_back(chip5);
        gameObjects.push_back(chip25);
        gameObjects.push_back(chip10);
        gameObjects.push_back(chip100);
        gameObjects.push_back(chip500);
        gameObjects.push_back(chip1000);
        gameObjects.push_back(chip5000);
        gameObjects.push_back(chip10000);
        gameObjects.push_back(chip25000);

        gameObjects.push_back(resetBetBtn);
        gameObjects.push_back(dealBtn);
        gameObjects.push_back(backToMenuBtn);
        gameObjects.push_back(bailoutBtn);

        gameObjects.push_back(hitBtn);
        gameObjects.push_back(standBtn);
        gameObjects.push_back(splitBtn);

        gameObjects.push_back(lifelineBtn);
        gameObjects.push_back(peekBtn);
        gameObjects.push_back(swapBtn);
        gameObjects.push_back(peekDeckBtn);
        gameObjects.push_back(peekDealerBtn);
        gameObjects.push_back(cancelPeekBtn);

        gameObjects.push_back(newRoundBtn);

        gameObjects.push_back(bg1Btn);
        gameObjects.push_back(bg2Btn);
        gameObjects.push_back(tableGreenBtn);
        gameObjects.push_back(tableBlueBtn);
        gameObjects.push_back(tableRedBtn);
        gameObjects.push_back(cbRedBtn);
        gameObjects.push_back(cbBlueBtn);
        gameObjects.push_back(cbGreenBtn);
        gameObjects.push_back(fullscreenBtn);
        gameObjects.push_back(backFromSettingsBtn);

        gameObjects.push_back(leaderboardBackBtn);
    };

    rebuildGameObjects();

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
        updateLogoTexture();
        deckPtr->setCardBackPath(currentCbPath);

        sf::Texture& chipsTex = texManager.get("textures/chips/Chips.png");
        chip1->setTexture(chipsTex);
        chip5->setTexture(chipsTex);
        chip25->setTexture(chipsTex);
        chip10->setTexture(chipsTex);
        chip100->setTexture(chipsTex);
        chip500->setTexture(chipsTex);
        chip1000->setTexture(chipsTex);
        chip5000->setTexture(chipsTex);
        chip10000->setTexture(chipsTex);
        chip25000->setTexture(chipsTex);

        sf::View newView(sf::FloatRect(0.f, 0.f, 1024.f, 768.f));
        gameView = getLetterboxView(newView, window.getSize().x, window.getSize().y);
        window.setView(gameView);
        
        rebuildGameObjects();
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
        deckPtr->reset();
        playerHands.clear();
        dealerHandPtr->cards.clear();
        lifelineUsed = false;
        peekUsed = false;
        // Resetowanie stanu uzycia umiejetnosci zamiany kart na poczatku rundy
        swapUsed = false;
        isPeekingChoiceActive = false;
        isDealerCardRevealed = false;

        playerHands.push_back(std::make_shared<Hand>(sf::Vector2f(350.f, 500.f)));

        activeHandIndex = 0;
        state = DEALING_START;
        dealStep = 0;
        dealTimer = 0.f;
        resultMessage = "";
        dealerTimer = 0.f;

        saveBalance(currentPlayerName, balance);
        rebuildGameObjects();
    };

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Event event;

        // SPRAWDZANIE CZY KARTA JEST W RUCHU
        bool isAnyAnimating = false;
        for (const auto& c : dealerHandPtr->cards) if (c.isAnimating) isAnyAnimating = true;
        for (const auto& h : playerHands)
            for (const auto& c : h->cards) if (c.isAnimating) isAnyAnimating = true;

        sf::Vector2i mousePosI = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePosI);

        // AKTUALIZACJA PRZYCISKOW
        if (state == BETTING) {
            chip1->setEnabled(balance >= 1);
            chip5->setEnabled(balance >= 5);
            chip25->setEnabled(balance >= 25);
            chip10->setEnabled(balance >= 10);
            chip100->setEnabled(balance >= 100);
            chip500->setEnabled(balance >= 500);
            chip1000->setEnabled(balance >= 1000);
            chip5000->setEnabled(balance >= 5000);
            chip10000->setEnabled(balance >= 10000);
            chip25000->setEnabled(balance >= 25000);
            resetBetBtn->setEnabled(currentBet > 0);
            dealBtn->setEnabled(currentBet > 0);
            bailoutBtn->setVisible(balance == 0 && currentBet == 0);
        } else if (state == PLAYER_TURN) {
            if (!playerHands.empty() && activeHandIndex < playerHands.size()) {
                auto& currentHand = playerHands[activeHandIndex];
                bool canSplit = (playerHands.size() == 1 && currentHand->cards.size() == 2 &&
                                 currentHand->cards[0].value == currentHand->cards[1].value &&
                                 balance >= currentBet);

                hitBtn->setEnabled(currentHand->getTotal() <= 21 && !isAnyAnimating && !isPeekingChoiceActive);
                standBtn->setEnabled(!isAnyAnimating && !isPeekingChoiceActive);
                splitBtn->setEnabled(canSplit && !isAnyAnimating && !isPeekingChoiceActive);

                if (hasSkillsMode) {
                    int skillCost = std::max(1, currentBet / 4);
                    lifelineBtn->setText("Minus card -" + std::to_string(skillCost) + "$");
                    peekBtn->setText("Sneaky peeky -" + std::to_string(skillCost) + "$");
                    
                    // Obliczenie kosztu i aktualizacja przycisku zamiany kart
                    int swapCost = std::max(1, currentBet / 2);
                    swapBtn->setText("Zamiana kart -" + std::to_string(swapCost) + "$");

                    lifelineBtn->setEnabled(currentHand->getTotal() > 21 && !lifelineUsed && balance >= skillCost && !isAnyAnimating && !isPeekingChoiceActive);
                    peekBtn->setEnabled(!peekUsed && balance >= skillCost && currentHand->getTotal() <= 21 && !isAnyAnimating && !isPeekingChoiceActive);
                    swapBtn->setEnabled(!swapUsed && balance >= swapCost && !isAnyAnimating && !isPeekingChoiceActive);
                    
                    if (isPeekingChoiceActive) {
                        peekDeckBtn->setVisible(true);
                        peekDealerBtn->setVisible(true);
                        cancelPeekBtn->setVisible(true);
                        lifelineBtn->setVisible(false);
                        peekBtn->setVisible(false);
                        swapBtn->setVisible(false);
                    } else {
                        peekDeckBtn->setVisible(false);
                        peekDealerBtn->setVisible(false);
                        cancelPeekBtn->setVisible(false);
                        lifelineBtn->setVisible(true);
                        peekBtn->setVisible(true);
                        swapBtn->setVisible(true);
                    }
                } else {
                    lifelineBtn->setVisible(false);
                    peekBtn->setVisible(false);
                    swapBtn->setVisible(false);
                    peekDeckBtn->setVisible(false);
                    peekDealerBtn->setVisible(false);
                    cancelPeekBtn->setVisible(false);
                }
            }
        }

        // AKTUALIZACJA MYSZY NA PRZYCISKACH
        for (auto& obj : gameObjects) {
            if (obj->shouldDraw(state)) {
                obj->updateMouse(mousePos);
            }
        }

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::Resized) {
                sf::View newView(sf::FloatRect(0.f, 0.f, 1024.f, 768.f));
                gameView = getLetterboxView(newView, event.size.width, event.size.height);
                window.setView(gameView);
            }

            // WPISYWANIE TEKSTU DLA NOWEGO PROFILU
            if (state == PROFILE_CREATE && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8) { // BACKSPACE
                    if (!typedName.empty()) typedName.pop_back();
                } else if (event.text.unicode == 13) { // ENTER
                    if (!typedName.empty()) {
                        currentPlayerName = typedName;
                        balance = 1000;
                        updateLeaderboard(currentPlayerName, balance);
                        loadBalance(currentPlayerName, balance);
                        state = GAMEPLAY_OPTIONS;
                        updateGameplayOptionsLabels();
                    }
                } else if (event.text.unicode >= 32 && event.text.unicode < 128 && typedName.length() < 15) {
                    typedName += static_cast<char>(event.text.unicode);
                }
            }

            bool mouseClicked = (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left);

            if (mouseClicked) {
                bool needsRebuild = false;
                //OBSLUGA KLIKNIEC
                for (auto& obj : gameObjects) {
                    if (obj->shouldDraw(state)) {
                        auto btn = std::dynamic_pointer_cast<Button>(obj);
                        if (btn && btn->getEnabled()) {
                            if (btn->isPressed(mousePos, mouseClicked)) {
                                if (state == PROFILE_SELECT) {
                                    if (btn == createProfileBtn) {
                                        typedName = "";
                                        state = PROFILE_CREATE;
                                    } else if (btn == backToMenuFromProfileBtn) {
                                        state = MENU;
                                        needsRebuild = true;
                                    } else {
                                        for (size_t i = 0; i < deleteProfileBtns.size(); ++i) {
                                            if (btn == deleteProfileBtns[i]) {
                                                std::string nameToRemove = profileBtns[i]->getTextString();
                                                auto scores = loadHighscores();
                                                scores.erase(std::remove_if(scores.begin(), scores.end(), [&](const Score& s){ return s.name == nameToRemove; }), scores.end());
                                                saveHighscores(scores);
                                                std::filesystem::remove("Save/savegame_" + nameToRemove + ".txt");
                                                needsRebuild = true;
                                                break;
                                            }
                                        }
                                        if (needsRebuild) break;

                                        for (const auto& pBtn : profileBtns) {
                                            if (btn == pBtn) {
                                                currentPlayerName = btn->getTextString();
                                                loadBalance(currentPlayerName, balance);
                                                state = GAMEPLAY_OPTIONS;
                                                updateGameplayOptionsLabels();
                                                break;
                                            }
                                        }
                                    }
                                }
                                else if (btn == playBtn) {
                                    if (profileBtns.empty()) {
                                        typedName = "";
                                        state = PROFILE_CREATE;
                                    } else {
                                        state = PROFILE_SELECT;
                                    }
                                    needsRebuild = true;
                                }
                                else if (btn == backFromProfileCreateBtn) {
                                    state = profileBtns.empty() ? MENU : PROFILE_SELECT;
                                    needsRebuild = true;
                                }
                                else if (btn == settingsBtn) {
                                    state = SETTINGS;
                                    updateSettingsButtonLabels();
                                }
                                else if (btn == leaderboardBtn) {
                                    state = LEADERBOARD;
                                }
                                else if (btn == exitBtn) {
                                    window.close();
                                }
                                else if (btn == leaderboardBackBtn) {
                                    state = MENU;
                                    needsRebuild = true;
                                }
                                else if (btn == modeClassicBtn) {
                                    hasSkillsMode = false;
                                    updateGameplayOptionsLabels();
                                }
                                else if (btn == modeCustomBtn) {
                                    hasSkillsMode = true;
                                    updateGameplayOptionsLabels();
                                }
                                else if (btn == diffNormalBtn) {
                                    isHardMode = false;
                                    updateGameplayOptionsLabels();
                                }
                                else if (btn == diffHardBtn) {
                                    isHardMode = true;
                                    updateGameplayOptionsLabels();
                                }
                                else if (btn == startGameBtn) {
                                    state = BETTING;
                                    saveBalance(currentPlayerName, balance);
                                }
                                else if (btn == backToMenuFromOptionsBtn) {
                                    state = MENU;
                                    needsRebuild = true;
                                }
                                else if (btn == chip1 && balance >= 1) { currentBet += 1; balance -= 1; handleSound.play(); }
                                else if (btn == chip5 && balance >= 5) { currentBet += 5; balance -= 5; handleSound.play(); }
                                else if (btn == chip25 && balance >= 25) { currentBet += 25; balance -= 25; handleSound.play(); }
                                else if (btn == chip10 && balance >= 10) { currentBet += 10; balance -= 10; handleSound.play(); }
                                else if (btn == chip100 && balance >= 100) { currentBet += 100; balance -= 100; handleSound.play(); }
                                else if (btn == chip500 && balance >= 500) { currentBet += 500; balance -= 500; handleSound.play(); }
                                else if (btn == chip1000 && balance >= 1000) { currentBet += 1000; balance -= 1000; handleSound.play(); }
                                else if (btn == chip5000 && balance >= 5000) { currentBet += 5000; balance -= 5000; handleSound.play(); }
                                else if (btn == chip10000 && balance >= 10000) { currentBet += 10000; balance -= 10000; handleSound.play(); }
                                else if (btn == chip25000 && balance >= 25000) { currentBet += 25000; balance -= 25000; handleSound.play(); }
                                else if (btn == resetBetBtn) {
                                    balance += currentBet;
                                    currentBet = 0;
                                    saveBalance(currentPlayerName, balance);
                                    handleSound.play();
                                }
                                else if (btn == backToMenuBtn) {
                                    state = MENU;
                                    needsRebuild = true;
                                    saveBalance(currentPlayerName, balance);
                                }
                                else if (btn == dealBtn && currentBet > 0) {
                                    startRound();
                                }
                                else if (btn == bailoutBtn && balance == 0 && currentBet == 0) {
                                    balance = 1000;
                                    saveBalance(currentPlayerName, balance);
                                    handleSound.play();
                                }
                                else if (state == PLAYER_TURN && !isAnyAnimating) {
                                    auto& currentHand = playerHands[activeHandIndex];
                                    if (btn == hitBtn && currentHand->getTotal() <= 21) {
                                        currentHand->addCard(deckPtr->drawCard());
                                        placeSound.play();
                                        if (currentHand->getTotal() > 21) {
                                            int skillCost = std::max(1, currentBet / 4);
                                            int swapCost = std::max(1, currentBet / 2);
                                            bool canUseLifeline = !lifelineUsed && balance >= skillCost;
                                            bool canUseSwap = !swapUsed && balance >= swapCost;
                                            if (!hasSkillsMode || (!canUseLifeline && !canUseSwap)) {
                                                activeHandIndex++;
                                            }
                                        }
                                    }
                                    else if (btn == standBtn) {
                                        activeHandIndex++;
                                    }
                                    else if (btn == splitBtn) {
                                        bool canSplit = (playerHands.size() == 1 && currentHand->cards.size() == 2 &&
                                                         currentHand->cards[0].value == currentHand->cards[1].value &&
                                                         balance >= currentBet);
                                        if (canSplit) {
                                            balance -= currentBet;
                                            currentBet *= 2;
                                            Card splitCard = currentHand->cards.back();
                                            currentHand->cards.pop_back();

                                            currentHand->startPos = sf::Vector2f(150.f, 500.f);
                                            currentHand->updateCardPositions();

                                            auto secondHand = std::make_shared<Hand>(sf::Vector2f(550.f, 500.f));
                                            splitCard.currentPos = currentHand->cards[0].currentPos;
                                            secondHand->addCard(splitCard);
                                            placeSound.play();

                                            playerHands.push_back(secondHand);
                                            rebuildGameObjects();
                                        }
                                    }
                                    else if (hasSkillsMode) {
                                        int skillCost = std::max(1, currentBet / 4);
                                        // Zdefiniowanie kosztu zamiany kart
                                        int swapCost = std::max(1, currentBet / 2);
                                        if (isPeekingChoiceActive) {
                                            if (btn == peekDeckBtn && balance >= skillCost) {
                                                balance -= skillCost;
                                                peekUsed = true;
                                                deckPtr->activatePeek();
                                                isPeekingChoiceActive = false;
                                                handleSound.play();
                                            }
                                            else if (btn == peekDealerBtn && balance >= skillCost) {
                                                balance -= skillCost;
                                                peekUsed = true;
                                                isDealerCardRevealed = true;
                                                isPeekingChoiceActive = false;
                                                handleSound.play();
                                            }
                                            else if (btn == cancelPeekBtn) {
                                                isPeekingChoiceActive = false;
                                            }
                                        } else {
                                            if (btn == lifelineBtn && currentHand->getTotal() > 21 && !lifelineUsed && balance >= skillCost) {
                                                balance -= skillCost;
                                                lifelineUsed = true;
                                                Card lifelineCard = deckPtr->drawCard();
                                                lifelineCard.isSubstracting = true;
                                                lifelineCard.frontSprite.setTexture(texManager.getInverted(lifelineCard.texturePath));
                                                currentHand->addCard(lifelineCard);
                                                placeSound.play();
                                                if (currentHand->getTotal() > 21) {
                                                    activeHandIndex++;
                                                }
                                            }
                                            else if (btn == peekBtn && !peekUsed && balance >= skillCost && currentHand->getTotal() <= 21) {
                                                isPeekingChoiceActive = true;
                                            }
                                            // Obsluga uzycia umiejetnosci "Zamiana kart" - zamiana kart gracza z krupierem
                                            else if (btn == swapBtn && !swapUsed && balance >= swapCost) {
                                                balance -= swapCost;
                                                swapUsed = true;
                                                std::swap(currentHand->cards, dealerHandPtr->cards);
                                                currentHand->updateCardPositions();
                                                dealerHandPtr->updateCardPositions();
                                                handleSound.play();
                                                if (currentHand->getTotal() > 21) {
                                                    int currentSkillCost = std::max(1, currentBet / 4);
                                                    if (!hasSkillsMode || lifelineUsed || balance < currentSkillCost) {
                                                        activeHandIndex++;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                else if (btn == bg1Btn) {
                                    currentBgPath = "textures/Backgrounds/background_1.png";
                                    updateBgTexture(currentBgPath);
                                    saveGlobalSettings(currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                                    updateSettingsButtonLabels();
                                }
                                else if (btn == bg2Btn) {
                                    currentBgPath = "textures/Backgrounds/background_2.png";
                                    updateBgTexture(currentBgPath);
                                    saveGlobalSettings(currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                                    updateSettingsButtonLabels();
                                }
                                else if (btn == tableGreenBtn) {
                                    currentTablePath = "textures/Tables/table_green.png.png";
                                    updateTableTexture(currentTablePath);
                                    saveGlobalSettings(currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                                    updateSettingsButtonLabels();
                                }
                                else if (btn == tableBlueBtn) {
                                    currentTablePath = "textures/Tables/table_blue.png";
                                    updateTableTexture(currentTablePath);
                                    saveGlobalSettings(currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                                    updateSettingsButtonLabels();
                                }
                                else if (btn == tableRedBtn) {
                                    currentTablePath = "textures/Tables/table_red.png";
                                    updateTableTexture(currentTablePath);
                                    saveGlobalSettings(currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                                    updateSettingsButtonLabels();
                                }
                                else if (btn == cbRedBtn) {
                                    currentCbPath = "textures/cardback/cardBackRed.png";
                                    deckPtr->setCardBackPath(currentCbPath);
                                    saveGlobalSettings(currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                                    updateSettingsButtonLabels();
                                }
                                else if (btn == cbBlueBtn) {
                                    currentCbPath = "textures/cardback/cardBackBlue.png";
                                    deckPtr->setCardBackPath(currentCbPath);
                                    saveGlobalSettings(currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                                    updateSettingsButtonLabels();
                                }
                                else if (btn == cbGreenBtn) {
                                    currentCbPath = "textures/cardback/cardBackGreen.png";
                                    deckPtr->setCardBackPath(currentCbPath);
                                    saveGlobalSettings(currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                                    updateSettingsButtonLabels();
                                }
                                else if (btn == fullscreenBtn) {
                                    isFullscreen = !isFullscreen;
                                    toggleWindowMode(isFullscreen);
                                    saveGlobalSettings(currentBgPath, currentTablePath, currentCbPath, isFullscreen);
                                    updateSettingsButtonLabels();
                                }
                                else if (btn == backFromSettingsBtn) {
                                    state = MENU;
                                    needsRebuild = true;
                                }
                                else if (btn == newRoundBtn) {
                                    currentBet = 0;
                                    state = BETTING;
                                }
                                break;
                            }
                        }
                    }
                }
                
                if (needsRebuild) {
                    rebuildProfileButtons();
                    rebuildGameObjects();
                }

                if (state == PLAYER_TURN && activeHandIndex >= playerHands.size()) {
                    bool allBusted = true;
                    for (const auto& h : playerHands) if (h->getTotal() <= 21) allBusted = false;
                    state = allBusted ? GAME_OVER : DEALER_TURN;
                }
            }
        }

        // AKTUALIZACJA STANU GRY
        for (auto& obj : gameObjects) {
            if (obj->shouldDraw(state)) {
                obj->update(dt);
            }
        }

        if (state == DEALING_START && !isAnyAnimating) {
            dealTimer += dt;
            if (dealTimer >= 0.35f) {
                dealTimer = 0.f;
                if (dealStep == 0) {
                    playerHands[0]->addCard(deckPtr->drawCard());
                    placeSound.play();
                    dealStep++;
                } else if (dealStep == 1) {
                    dealerHandPtr->addCard(deckPtr->drawCard());
                    placeSound.play();
                    dealStep++;
                } else if (dealStep == 2) {
                    playerHands[0]->addCard(deckPtr->drawCard());
                    placeSound.play();
                    dealStep++;
                } else if (dealStep == 3) {
                    dealerHandPtr->addCard(deckPtr->drawCard());
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
                if (dealerHandPtr->getTotal() < dealerLimit) {
                    dealerHandPtr->addCard(deckPtr->drawCard());
                    placeSound.play();
                } else {
                    state = GAME_OVER;
                }
            }
        }

        if (state == GAME_OVER && resultMessage.empty()) {
            int dTotal = dealerHandPtr->getTotal();
            float betPerHand = (playerHands.size() > 1) ? (currentBet / 2.f) : (float)currentBet;

            for (size_t i = 0; i < playerHands.size(); ++i) {
                int pTotal = playerHands[i]->getTotal();
                bool isBlackjack = (playerHands.size() == 1 && playerHands[i]->cards.size() == 2 && pTotal == 21);
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
            // ZAPIS GRY DO TABELI WYNIKOW I PLIKU SAVEGAME
            saveBalance(currentPlayerName, balance);
            updateLeaderboard(currentPlayerName, balance);
            
            rebuildProfileButtons();
            rebuildGameObjects();
        }

        window.clear();
        window.draw(backgroundSprite);
        window.draw(tableSprite);
        if (state == MENU) {
            window.draw(logoSprite);
        }

        std::string ecoStr = "Balance: $" + std::to_string(balance) + "\nBet: $" + std::to_string(currentBet) + "\n\n";

        if (state == MENU) {
            // No redundant white text "BLACKJACK PP" drawn on top of the table logo
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

            sf::Text diffLabel("SELECT DIFFICULTY:", font, 18);
            diffLabel.setFillColor(sf::Color::White);
            sf::FloatRect diffRect = diffLabel.getLocalBounds();
            diffLabel.setOrigin(diffRect.left + diffRect.width / 2.f, 0.f);
            diffLabel.setPosition(512.f, 290.f);
            window.draw(diffLabel);
        }
        else if (state == BETTING) {
            uiText.setString(ecoStr + "PLACE YOUR BET:");
            sf::FloatRect textRect = uiText.getLocalBounds();
            uiText.setOrigin(textRect.left + textRect.width / 2.f, 0.f);
            uiText.setPosition(512.f, 150.f);
            window.draw(uiText);
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

            sf::Text tableLabel("SELECT TABLE:", font, 18);
            tableLabel.setFillColor(sf::Color::White);
            sf::FloatRect tableRect = tableLabel.getLocalBounds();
            tableLabel.setOrigin(tableRect.left + tableRect.width / 2.f, 0.f);
            tableLabel.setPosition(512.f, 220.f);
            window.draw(tableLabel);

            sf::Text cbLabel("SELECT CARD BACK:", font, 18);
            cbLabel.setFillColor(sf::Color::White);
            sf::FloatRect cbRect = cbLabel.getLocalBounds();
            cbLabel.setOrigin(cbRect.left + cbRect.width / 2.f, 0.f);
            cbLabel.setPosition(512.f, 310.f);
            window.draw(cbLabel);

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
        else if (state == LEADERBOARD) {
            // RYSOWANIE LABELI
            sf::RectangleShape frame(sf::Vector2f(500.f, 400.f));
            frame.setPosition(262.f, 150.f);
            frame.setFillColor(sf::Color(0, 0, 0, 150));
            frame.setOutlineColor(sf::Color(46, 204, 113));
            frame.setOutlineThickness(3.f);
            window.draw(frame);

            uiText.setString("LEADERBOARD");
            uiText.setCharacterSize(28);
            uiText.setFillColor(sf::Color::White);
            sf::FloatRect textRect = uiText.getLocalBounds();
            uiText.setOrigin(textRect.left + textRect.width / 2.f, 0.f);
            uiText.setPosition(512.f, 80.f);
            window.draw(uiText);
            uiText.setCharacterSize(20);

            auto scores = loadHighscores();
            std::sort(scores.begin(), scores.end(), [](const Score& a, const Score& b) {
                return a.value > b.value;
            });
            size_t count = std::min((size_t)10, scores.size());
            for (size_t i = 0; i < count; ++i) {
                sf::Text entryText;
                entryText.setFont(font);
                entryText.setCharacterSize(22);
                
                if (i == 0) entryText.setFillColor(sf::Color(255, 215, 0)); // ZLOTY KOLOR (1 GRACZ)
                else if (i == 1) entryText.setFillColor(sf::Color(192, 192, 192)); // SREBRYNY (2 GRACZ)
                else if (i == 2) entryText.setFillColor(sf::Color(205, 127, 50)); // BRAZOWY (3 GRACZ)
                else entryText.setFillColor(sf::Color::White);

                entryText.setString(std::to_string(i + 1) + ".  " + scores[i].name);
                entryText.setPosition(300.f, 180.f + i * 50.f);
                window.draw(entryText);

                sf::Text scoreValText;
                scoreValText.setFont(font);
                scoreValText.setCharacterSize(22);
                if (i == 0) scoreValText.setFillColor(sf::Color(255, 215, 0));
                else if (i == 1) scoreValText.setFillColor(sf::Color(192, 192, 192));
                else if (i == 2) scoreValText.setFillColor(sf::Color(205, 127, 50));
                else scoreValText.setFillColor(sf::Color::White);

                scoreValText.setString("$" + std::to_string(scores[i].value));
                sf::FloatRect valRect = scoreValText.getLocalBounds();
                scoreValText.setPosition(724.f - valRect.width, 180.f + i * 50.f);
                window.draw(scoreValText);
            }
        }
        else if (state == PROFILE_SELECT) {
            sf::Text titleText("SELECT PLAYER PROFILE", font, 36);
            titleText.setFillColor(sf::Color::White);
            sf::FloatRect ttRect = titleText.getLocalBounds();
            titleText.setOrigin(ttRect.left + ttRect.width / 2.f, 0.f);
            titleText.setPosition(512.f, 60.f);
            window.draw(titleText);
        }
        else if (state == PROFILE_CREATE) {
            sf::RectangleShape frame(sf::Vector2f(400.f, 200.f));
            frame.setPosition(312.f, 250.f);
            frame.setFillColor(sf::Color(0, 0, 0, 150));
            frame.setOutlineColor(sf::Color(46, 204, 113));
            frame.setOutlineThickness(3.f);
            window.draw(frame);

            sf::Text promptText("Enter player name:", font, 24);
            promptText.setFillColor(sf::Color::White);
            sf::FloatRect ptRect = promptText.getLocalBounds();
            promptText.setOrigin(ptRect.left + ptRect.width / 2.f, 0.f);
            promptText.setPosition(512.f, 280.f);
            window.draw(promptText);

            sf::Text nameText(typedName + "_", font, 28);
            nameText.setFillColor(sf::Color::White);
            sf::FloatRect ntRect = nameText.getLocalBounds();
            nameText.setOrigin(ntRect.left + ntRect.width / 2.f, 0.f);
            nameText.setPosition(512.f, 340.f);
            window.draw(nameText);
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
                skillsTitle.setFillColor(sf::Color::White);
                sf::FloatRect stRect = skillsTitle.getLocalBounds();
                skillsTitle.setOrigin(stRect.left + stRect.width / 2.f, 0.f);
                skillsTitle.setPosition(917.f, 130.f);
                window.draw(skillsTitle);
            }

            std::string statusStr = ecoStr + "Dealer:\nTotal: ";
            if (state != PLAYER_TURN && state != DEALING_START || isDealerCardRevealed) statusStr += std::to_string(dealerHandPtr->getTotal()) + "\n";
            else statusStr += "?\n";

            statusStr += "\nYou:\n";
            for (size_t i = 0; i < playerHands.size(); ++i) {
                if (playerHands.size() > 1) statusStr += "Hand " + std::to_string(i + 1) + " ";
                if ((state == PLAYER_TURN || state == DEALING_START) && i == activeHandIndex) statusStr += "(Active) ";
                statusStr += "Total: " + std::to_string(playerHands[i]->getTotal()) + "\n";
            }

            statusStr += "\n";

            if (state == PLAYER_TURN) {
                if (isAnyAnimating) {
                    statusStr += "Dealing...";
                } else {
                    if (playerHands[activeHandIndex]->getTotal() > 21){
                        statusStr += "BUSTED!\n";
                    }else{
                         statusStr += "Choose action...";
                    }
                    if (deckPtr->getIsPeeking()) {
                        statusStr += "\nNext card: [ "
                                     + deckPtr->getPeekDescription()
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
        }

        dealerHandPtr->hideFirstCard = (state == PLAYER_TURN || state == DEALING_START) && !isDealerCardRevealed;
        dealerHandPtr->transparentFirstCard = isDealerCardRevealed && (state == PLAYER_TURN || state == DEALING_START);

        for (auto& obj : gameObjects) {
            if (obj->shouldDraw(state)) {
                obj->draw(window);
            }
        }

        window.display();
    }

    return 0;
}
