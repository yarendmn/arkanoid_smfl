#ifndef POWERUP_HPP
#define POWERUP_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "definitions.hpp"

// SFML 3.0 uyumlu enum class
enum class PowerUpType {
    ExpandPaddle,
    SlowBall,
    SpeedBall
};

struct PowerUp {
    sf::Sprite sprite;
    PowerUpType type;

    PowerUp(const sf::Texture& texture, PowerUpType t)
        : sprite(texture), type(t) {}
};

// Fonksiyon imzaları
void spawnPowerUp(std::vector<PowerUp>& powerUps, const sf::Texture& tex, sf::Vector2f pos);
void updatePowerUps(std::vector<PowerUp>& powerUps, float dt, sf::FloatRect paddleBounds);

#endif // POWERUP_HPP