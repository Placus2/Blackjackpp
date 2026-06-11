#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class TextureManager {
private:
    std::map<std::string, sf::Texture> textures;
public:
    sf::Texture& get(const std::string& path);
    sf::Texture& getInverted(const std::string& path);
    void clear();
};


extern TextureManager texManager;
