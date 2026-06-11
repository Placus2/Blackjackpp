#pragma once
#include <SFML/Graphics.hpp>


enum GameState { PROFILE_SELECT, PROFILE_CREATE, MENU, GAMEPLAY_OPTIONS, BETTING, DEALING_START, PLAYER_TURN, DEALER_TURN, GAME_OVER, SETTINGS, LEADERBOARD };

class GameObject {
public:
    virtual ~GameObject() = default;
    
    virtual void draw(sf::RenderWindow& window) = 0;
    
    virtual void update(float dt) {}
    
    virtual void updateMouse(const sf::Vector2f& mousePos) {}
    
    virtual bool shouldDraw(GameState state) const = 0;
};
