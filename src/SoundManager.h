#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SFML/Audio.hpp>
#include <map>
#include <string>

enum class SoundType {
    FAN,
    PLACE,
    HANDLE,
    CLICK
};

class SoundManager {
public:
    SoundManager();
    void play(SoundType type);

private:
    std::map<SoundType, sf::SoundBuffer> buffers;
    std::map<SoundType, sf::Sound> sounds;

    void loadSound(SoundType type, const std::string& path);
};

#endif // SOUNDMANAGER_H
