#include "Card.h"
#include <cmath>

void Card::updateAnimation(float dt) {
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
