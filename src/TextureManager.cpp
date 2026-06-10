#include "TextureManager.h"
#include <iostream>

TextureManager texManager;

sf::Texture& TextureManager::get(const std::string& path) {
    if (textures.find(path) == textures.end()) {
        if (!textures[path].loadFromFile(path)) {
            std::cerr << "Blad ladowania tekstury: " << path << std::endl;
        }
    }
    return textures[path];
}
