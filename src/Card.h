#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct Card {
    int value;
    std::string rank;
    std::string texturePath;
    sf::Sprite frontSprite;
    sf::Sprite backSprite;

    sf::Vector2f currentPos;
    sf::Vector2f targetPos;
    bool isAnimating = false;
    bool isSubstracting = false;

    void updateAnimation(float dt);
};
