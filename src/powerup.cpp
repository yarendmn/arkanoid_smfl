#include "powerup.hpp"
#include <cstdlib>


void spawnPowerUp(std::vector<PowerUp>& powerUps, const sf::Texture& tex, sf::Vector2f pos) {
    if (rand() % 100 < 25) { 
        int rType = rand() % 4; // 0-3 arasında rastgele sayı
        PowerUpType pType = static_cast<PowerUpType>(rType);
        
        PowerUp power(tex, pType);
        
        // SFML 3.0 Renk Ayarları
        if (pType == PowerUpType::ExpandPaddle) power.sprite.setColor(sf::Color::Green);
        else if (pType == PowerUpType::SlowBall) power.sprite.setColor(sf::Color(255, 165, 0)); // Turuncu
        else if (pType == PowerUpType::SpeedBall) power.sprite.setColor(sf::Color(255, 0, 255)); // Magenta
        else power.sprite.setColor(sf::Color::Yellow); // MultiBall sarı olsun  
        
        power.sprite.setPosition(pos);
        powerUps.push_back(power);
    }
}

void updatePowerUps(std::vector<PowerUp>& powerUps, float dt, sf::FloatRect paddleBounds) {
    for (auto it = powerUps.begin(); it != powerUps.end(); ) {
        // SFML 3.0 hareket mantığı
        it->sprite.move({0.f, 200.f * dt});

        // SFML 3.0 Intersection Kontrolü
        if (it->sprite.getGlobalBounds().findIntersection(paddleBounds)) {
            // Yakalandı! (Etki main.cpp'de currentTime üzerinden yönetilecek)
            // it = powerUps.erase(it); yerine main'de yönetmek daha iyidir 
            // ama şimdilik burada siliyoruz.
            it = powerUps.erase(it);
        } 
        else if (it->sprite.getPosition().y > WINDOW_HEIGHT) {
            it = powerUps.erase(it);
        } 
        else {
            ++it;
        }
    }
}