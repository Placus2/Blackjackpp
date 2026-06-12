#include "SoundManager.h"
#include <iostream>

// Wczytuje podstawowe dzwieki
SoundManager::SoundManager() {
    loadSound(SoundType::FAN, "sounds/card-fan-1.ogg");
    loadSound(SoundType::PLACE, "sounds/card-place-1.ogg");
    loadSound(SoundType::HANDLE, "sounds/chips-handle-6.ogg");
    loadSound(SoundType::CLICK, "sounds/chip-lay-1.ogg");
}

// Wczytuje pojedynczy dzwiek z pliku
void SoundManager::loadSound(SoundType type, const std::string& path) {
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(path)) {
        buffers[type] = buffer;
        sounds[type].setBuffer(buffers[type]);
    } else {
        std::cerr << "Error loading sound: " << path << std::endl;
    }
}

// Odtwarza wybrany dzwiek
void SoundManager::play(SoundType type) {
    auto it = sounds.find(type);
    if (it != sounds.end()) {
        it->second.play();
    }
}
