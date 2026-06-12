#include "Card.h"
#include <cmath>

// Oblicza i aktualizuje rotacje i przesunięcie animacji karty
void Card::updateAnimation(float dt) {
    if (!isAnimating) return;

    sf::Vector2f dir = targetPos - currentPos;
    float distance = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    float speed = 2500.f;

    if (distance < speed * dt) {
        currentPos = targetPos;
        isAnimating = false;
        frontSprite.setRotation(0.f);
        backSprite.setRotation(0.f);
    } else {
        currentPos += (dir / distance) * speed * dt;
        
        float rotSpeed = 360.f; // 360 stopni na sekunde
        frontSprite.rotate(rotSpeed * dt);
        backSprite.rotate(rotSpeed * dt);
    }

    frontSprite.setPosition(currentPos);
    backSprite.setPosition(currentPos);
}
