#include "TextureManager.h"
#include <iostream>

TextureManager texManager;

sf::Texture& TextureManager::get(const std::string& path) {
    auto it = textures.find(path);
    if (it == textures.end()) {
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            textures[path] = std::move(tex);
        } else {
            std::cerr << "Error loading texture: " << path << std::endl;
        }
    }
    return textures[path];
}

sf::Texture& TextureManager::getInverted(const std::string& path) {
    std::string invertedPath = path + "_inverted";
    auto it = textures.find(invertedPath);
    if (it == textures.end()) {
        sf::Image img;
        if (img.loadFromFile(path)) {
            sf::Vector2u size = img.getSize();
            for (unsigned int y = 0; y < size.y; ++y) {
                for (unsigned int x = 0; x < size.x; ++x) {
                    sf::Color c = img.getPixel(x, y);
                    img.setPixel(x, y, sf::Color(255 - c.r, 255 - c.g, 255 - c.b, c.a));
                }
            }
            sf::Texture tex;
            if (tex.loadFromImage(img)) {
                textures[invertedPath] = std::move(tex);
            }
        } else {
            std::cerr << "Error loading texture for inversion: " << path << std::endl;
        }
    }
    return textures[invertedPath];
}

void TextureManager::clear() {
    textures.clear();
}
