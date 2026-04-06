#ifndef ZSH_BREAKOUT_GAMELEVEL_H
#define ZSH_BREAKOUT_GAMELEVEL_H

#include <vector>

class GameLevel {
public:
    using uint = unsigned int;

    enum class TileType : uint {
        NONE = 0,
        SOLID = 1,
        UN_SOLID = 2
    };

private:
    std::vector<class GameObject> Bricks;

public:
    GameLevel();

    void Load(const std::string& file, uint levelWidth, uint levelHeight);

    void Draw(class SpriteRenderer& renderer);

    bool isCompleted();

private:
    void init(const std::vector<std::vector<uint>>& tileData, uint levelWidth, uint levelHeight);
};

#endif