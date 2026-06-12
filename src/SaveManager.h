#ifndef SAVEMANAGER_H
#define SAVEMANAGER_H

#include <string>
#include <vector>

// TABELA WYNIKÓW
struct Score {
    std::string name;
    int value;
};

// Zapisuje najlepsze wyniki graczy do pliku tekstowego
void saveHighscores(const std::vector<Score>& scores);

// Wczytuje najlepsze wyniki graczy z pliku tekstowego
std::vector<Score> loadHighscores();

// Aktualizuje wynik gracza na liscie najlepszych wynikow i zapisuje dane
void updateLeaderboard(const std::string& playerName, int newBalance);

// Zapisuje globalne ustawienia gry (tlo, stol, rewers, pelny ekran, jezyk) do pliku
void saveGlobalSettings(const std::string& bgPath, const std::string& tablePath, const std::string& cbPath, bool isFullscreen, bool isPolish);

// Wczytuje globalne ustawienia gry z pliku, lub ustawia domyslne
void loadGlobalSettings(std::string& bgPath, std::string& tablePath, std::string& cbPath, bool& isFullscreen, bool& isPolish);

// Zapisuje aktualne saldo punktow/pieniedzy gracza
void saveBalance(const std::string& playerName, int balance);

// Wczytuje saldo punktow/pieniedzy gracza na podstawie jego nazwy
void loadBalance(const std::string& playerName, int& balance);

#endif // SAVEMANAGER_H
