#include "SaveManager.h"
#include <fstream>
#include <filesystem>
#include <iostream>

// Zapisuje tabele wynikow do pliku
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

// Odczytuje tabele wynikow z pliku
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

// Aktualizuje lub dodaje gracza do tabeli wynikow
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

// Zapisuje ustawienia gry do pliku
void saveGlobalSettings(const std::string& bgPath, const std::string& tablePath, const std::string& cbPath, bool isFullscreen, bool isPolish) {
    std::filesystem::create_directories("Save");
    std::ofstream file("Save/global_settings.txt");
    if (file.is_open()) {
        file << bgPath << "\n" << tablePath << "\n" << cbPath << "\n" << (isFullscreen ? 1 : 0) << "\n" << (isPolish ? 1 : 0) << "\n";
    }
}

// Odczytuje ustawienia gry z pliku lub wczytuje domyslne
void loadGlobalSettings(std::string& bgPath, std::string& tablePath, std::string& cbPath, bool& isFullscreen, bool& isPolish) {
    std::ifstream file("Save/global_settings.txt");
    bool success = false;
    if (file.is_open()) {
        std::vector<std::string> lines; std::string line;
        while (std::getline(file, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            lines.push_back(line);
        }
        if (lines.size() >= 5) {
            try {
                bgPath = lines[0]; tablePath = lines[1]; cbPath = lines[2];
                isFullscreen = (std::stoi(lines[3]) != 0);
                isPolish = (std::stoi(lines[4]) != 0);
                success = true;
            } catch (...) { success = false; }
        } else if (lines.size() == 4) {
            try {
                bgPath = lines[0]; tablePath = lines[1]; cbPath = lines[2];
                isFullscreen = (std::stoi(lines[3]) != 0);
                isPolish = false;
                success = true;
            } catch (...) { success = false; }
        }
    }
    if (!success) {
        bgPath = "textures/Backgrounds/background_1.png";
        tablePath = "textures/Tables/table_green.png.png";
        cbPath = "textures/cardback/cardBackRed.png";
        isFullscreen = false;
        isPolish = false;
        saveGlobalSettings(bgPath, tablePath, cbPath, isFullscreen, isPolish);
    }
}

// Zapisuje stan konta gracza
void saveBalance(const std::string& playerName, int balance) {
    if (playerName.empty()) return;
    updateLeaderboard(playerName, balance);
}

// Odczytuje stan konta gracza z tabeli wynikow
void loadBalance(const std::string& playerName, int& balance) {
    auto scores = loadHighscores();
    for (const auto& s : scores) {
        if (s.name == playerName) {
            balance = s.value;
            break;
        }
    }
}
