#include "SoundManager.h"
#include <iostream>

SoundManager::SoundManager() {
    loadSound(SoundType::FAN, "sounds/card-fan-1.ogg");
    loadSound(SoundType::PLACE, "sounds/card-place-1.ogg");
    loadSound(SoundType::HANDLE, "sounds/chips-handle-6.ogg");
    loadSound(SoundType::CLICK, "sounds/chip-lay-1.ogg");
}

void SoundManager::loadSound(SoundType type, const std::string& path) {
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(path)) {
        buffers[type] = buffer;
        sounds[type].setBuffer(buffers[type]);
    } else {
        std::cerr << "Error loading sound: " << path << std::endl;
    }
}

void SoundManager::play(SoundType type) {
    auto it = sounds.find(type);
    if (it != sounds.end()) {
        it->second.play();
    }
}
