#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct Card {
    int value;               // WARTOSC
    std::string rank;        // RANGA
    std::string texturePath; // SCIEZKA DO TEKSTUR
    sf::Sprite frontSprite;  // FRONT
    sf::Sprite backSprite;   // REWERS

    sf::Vector2f currentPos; // AKTUALNA POZYCJA
    sf::Vector2f targetPos;  // POZYCJA DOCELOWA
    bool isAnimating = false;    // RUCH
    bool isSubstracting = false; // ODEJMOWANIE

    void updateAnimation(float dt);
};
