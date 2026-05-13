#include "level.hpp"
#include <fstream>
#include <iostream>

void loadLevel(int levelNum, std::vector<GameBrick>& bricks, const sf::Texture& texture) {
    bricks.clear();
    std::string fileName = "assets/levels/level" + std::to_string(levelNum) + ".txt";
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << "Level dosyasi acilamadi: " << fileName << std::endl;
        return;
    }

    std::string line;
    int row = 0;
    while (std::getline(file, line)) {
        for (int col = 0; col < (int)line.size(); ++col) {
            if (line[col] == '0') continue;

            GameBrick brick(texture);
            brick.hp = (line[col] == '2') ? 2 : 1;
            
            if (brick.hp == 2) brick.sprite.setColor(sf::Color(150, 150, 150));

            brick.sprite.setScale({
                (float)BRICK_WIDTH / brick.sprite.getLocalBounds().size.x,
                (float)BRICK_HEIGHT / brick.sprite.getLocalBounds().size.y
            });

            brick.sprite.setPosition({
                (float)col * (BRICK_WIDTH + BRICK_OFFSET) + 15.f,
                (float)row * (BRICK_HEIGHT + BRICK_OFFSET) + 60.f
            });
            bricks.push_back(brick);
        }
        row++;
    }
    file.close();
}