#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "definitions.hpp"

// GameBrick struct'ını burada da kullanabilmek için main'den buraya taşıyabiliriz 
// ya da definitions.hpp içine koyabilirsin.
struct GameBrick; 

void loadLevel(int levelNum, std::vector<GameBrick>& bricks, const sf::Texture& texture);

#endif