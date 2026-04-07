#include <GameLevel.h>
#include <GameObject.h>
#include <fstream>
#include <Core/log.h>

GameLevel::GameLevel() {

}

void GameLevel::Load(const std::string& file, uint levelWidth, uint levelHeight) {
    this->Bricks.clear();

    uint tileCode;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<uint>> tileData;

    if (!fstream) {
        APP_ERROR("GameLevel: 加载文件时，文件不存在: {}", file);
        return; 
    }

    while (std::getline(fstream, line)) {
        std::istringstream sstream(line);
        std::vector<uint> row;
        while (sstream >> tileCode) {
            row.push_back(tileCode);
        }
        tileData.push_back(row);
    }

    if (tileData.size() > 0) {
        this->init(tileData, levelWidth, levelHeight);
    }
    else {
        APP_ERROR("GameLevel: 加载文件时，文件内容不足或内容错误: {}", file);
    }
}

void GameLevel::init(const std::vector<std::vector<uint>>& tileData, uint levelWidth, uint levelHeight) {
    uint height = tileData.size();
    uint width = tileData[0].size();

    float brisk_width = levelWidth  / static_cast<float>(width);
    float brisk_height = levelHeight / static_cast<float>(height);

    for (uint y = 0; y < height; ++y) {
        for (uint x = 0; x < width; ++x) {
            uint code = tileData[y][x];

            TileType type = TileType::NONE;
            
            if (code == 1) type = TileType::SOLID;
            else if (code > 1) type = TileType::UN_SOLID;

            if (type == TileType::NONE) continue;

            glm::vec2 pos(brisk_width * x, brisk_height * y);
            glm::vec2 size(brisk_width, brisk_height);

            if (type == TileType::SOLID) {
                GameObject obj(pos, size, "block_solid", glm::vec3(0.8f, 0.8f, 0.7f));
                obj.setSolid(true);
                this->Bricks.push_back(obj);
            }
            else if (type == TileType::UN_SOLID) {
                glm::vec3 color = glm::vec3(1.0f);
                if (code == 2)      color = glm::vec3(0.2f, 0.6f, 1.0f); // 蓝色
                else if (code == 3) color = glm::vec3(0.0f, 0.7f, 0.0f); // 绿色
                else if (code == 4) color = glm::vec3(0.8f, 0.8f, 0.4f); // 黄色
                else if (code == 5) color = glm::vec3(1.0f, 0.5f, 0.0f); // 橙色
           
                GameObject obj(pos, size, "block", color);

                obj.setSolid(false);
                this->Bricks.push_back(obj);
            }
        }
    }
}

void GameLevel::Draw(SpriteRenderer& renderer) {
    for (GameObject& tile : this->Bricks) {
        if (!tile.isDestroyed()) {
            tile.Draw(renderer);
        }
    }
}

bool GameLevel::isCompleted() {
    for (GameObject& tile : this->Bricks) {
        if (!tile.isSolid() && !tile.isDestroyed()) {
            return false;
        }
    }
    return true;
}