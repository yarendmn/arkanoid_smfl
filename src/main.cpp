#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <fstream> // dosya işlemleri için
#include "definitions.hpp"
#include "level.hpp"
#include "powerup.hpp"



int main() {

    srand(static_cast<unsigned>(time(nullptr)));

    sf::RenderWindow window(
        sf::VideoMode(
            {(unsigned)WINDOW_WIDTH, (unsigned)WINDOW_HEIGHT}
        ),
        "Arkanoid SFML 3"
    );

    window.setFramerateLimit(60);

    // ===================================================
    // FONT
    // ===================================================

    sf::Font font;

    if (!font.openFromFile("assets/fonts/font.ttf")) {
        std::cout << "Font yuklenemedi!\n";
    }

    // ===================================================
    // SCORE
    // ===================================================

    int score = 0;
    int level = 1;

    sf::Text scoreText(font);

    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({20.f, 10.f});

    // ===================================================
    // TEXTURES
    // ===================================================

    sf::Texture paddleTexture;
    sf::Texture ballTexture;
    sf::Texture brickTexture;
    sf::Texture powerTexture;

    paddleTexture.loadFromFile(
        "assets/textures/paddle.png"
    );

    ballTexture.loadFromFile(
        "assets/textures/ball.png"
    );

    brickTexture.loadFromFile(
        "assets/textures/brick.png"
    );

    if (!powerTexture.loadFromFile(
        "assets/textures/powerup.png"
    )) {

        sf::Image img({20, 20}, sf::Color::Green);
        powerTexture.loadFromImage(img);
    }

    // ===================================================
    // PADDLE
    // ===================================================

    sf::Sprite paddle(paddleTexture);

    paddle.setScale({
        (float)PADDLE_WIDTH /
        paddle.getLocalBounds().size.x,

        (float)PADDLE_HEIGHT /
        paddle.getLocalBounds().size.y
    });

    paddle.setPosition({
        (WINDOW_WIDTH -
        paddle.getGlobalBounds().size.x) / 2.f,

        WINDOW_HEIGHT - 50.f
    });

    // ===================================================
    // BALL
    // ===================================================

    sf::Sprite ball(ballTexture);

    ball.setPosition({
        WINDOW_WIDTH / 2.f,
        WINDOW_HEIGHT / 2.f
    });

    sf::Vector2f ballVelocity = {
        BALL_SPEED_X,
        BALL_SPEED_Y
    };

    // ===================================================
    // BRICKS
    // ===================================================

    std::vector<GameBrick> bricks;

    auto createLevel = [&](int currentLevel) {

        bricks.clear();

        loadLevel(currentLevel, bricks, brickTexture); // dosyadan okuma fonksiyonu
    };

    createLevel(level);

    // ===================================================
    // POWERUPS
    // ===================================================

    std::vector<PowerUp> powerUps;

    // ===================================================
    // CLOCKS
    // ===================================================

    sf::Clock deltaClock;
    sf::Clock gameClock;

    float paddleEffectEnd = 0.f;
    float speedEffectEnd = 0.f;

    // ===================================================
    // GAME LOOP
    // ===================================================

    while (window.isOpen()) {

        float dt =
            deltaClock.restart().asSeconds();

        float currentTime =
            gameClock.getElapsedTime().asSeconds();

        // ===============================================
        // EVENTS
        // ===============================================

        while (const auto event =
            window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // ===============================================
        // PADDLE MOVEMENT
        // ===============================================

        if (
            sf::Keyboard::isKeyPressed(
                sf::Keyboard::Key::Left
            )

            && paddle.getPosition().x > 0
        ) {

            paddle.move({
                -PADDLE_SPEED * dt,
                0.f
            });
        }

        if (
            sf::Keyboard::isKeyPressed(
                sf::Keyboard::Key::Right
            )

            && paddle.getPosition().x <
            WINDOW_WIDTH -
            paddle.getGlobalBounds().size.x
        ) {

            paddle.move({
                PADDLE_SPEED * dt,
                0.f
            });
        }

        // ===============================================
        // BALL MOVEMENT
        // ===============================================

        ball.move(ballVelocity * dt);

        // ===============================================
        // WALL COLLISION
        // ===============================================

        if (ball.getPosition().x <= 0) {

            ballVelocity.x =
                std::abs(ballVelocity.x);

            ball.setPosition({
                0.f,
                ball.getPosition().y
            });
        }

        if (
            ball.getPosition().x >=
            WINDOW_WIDTH -
            ball.getGlobalBounds().size.x
        ) {

            ballVelocity.x =
                -std::abs(ballVelocity.x);

            ball.setPosition({
                WINDOW_WIDTH -
                ball.getGlobalBounds().size.x,

                ball.getPosition().y
            });
        }

        if (ball.getPosition().y <= 0) {

            ballVelocity.y =
                std::abs(ballVelocity.y);

            ball.setPosition({
                ball.getPosition().x,
                0.f
            });
        }

        // ===============================================
        // BALL FALL
        // ===============================================

        if (ball.getPosition().y >
            WINDOW_HEIGHT) {

            ball.setPosition({
                WINDOW_WIDTH / 2.f,
                WINDOW_HEIGHT / 2.f
            });

            float speed =
                std::min(
                    200.f + level * 15.f,
                    400.f
                );

            ballVelocity = {
                speed,
                -speed
            };
        }

        // ===============================================
        // PADDLE COLLISION
        // ===============================================

        if (
            ball.getGlobalBounds().findIntersection(
                paddle.getGlobalBounds()
            )
        ) {

            ballVelocity.y =
                -std::abs(ballVelocity.y);

            // top paddle içine girmesin
            ball.setPosition({
                ball.getPosition().x,

                paddle.getPosition().y
                - ball.getGlobalBounds().size.y
                - 1.f
            });

            // açı sistemi
            float paddleCenter =
                paddle.getPosition().x +
                paddle.getGlobalBounds().size.x / 2.f;

            float ballCenter =
                ball.getPosition().x +
                ball.getGlobalBounds().size.x / 2.f;

            float distance =
                ballCenter - paddleCenter;

            ballVelocity.x =
                distance * 5.f;
        }

        // ===============================================
        // BRICK COLLISION
        // ===============================================

        for (
            auto it = bricks.begin();
            it != bricks.end();
        ) {

            if (
                ball.getGlobalBounds()
                .findIntersection(
                    it->sprite.getGlobalBounds()
                )
            ) {

                ballVelocity.y =
                    -ballVelocity.y;

                // top brick içine girmesin
                if (ballVelocity.y > 0) {

                    ball.setPosition({
                        ball.getPosition().x,

                        it->sprite.getPosition().y +
                        it->sprite.getGlobalBounds().size.y +
                        1.f
                    });

                } else {

                    ball.setPosition({
                        ball.getPosition().x,

                        it->sprite.getPosition().y -
                        ball.getGlobalBounds().size.y -
                        1.f
                    });
                }

                it->hp--;

                if (it->hp <= 0) {

                   spawnPowerUp(powerUps, powerTexture, it->sprite.getPosition());

                    it = bricks.erase(it);

                    score += 10;

                } else {

                    it->sprite.setColor(
                        sf::Color::White
                    );

                    ++it;
                }

                break;

            } else {
                ++it;
            }
        }

        // ===============================================
        // POWERUPS bas
        // ===============================================

// ===============================================
        // POWERUPS (GÜNCELLENMİŞ VE EFEKT BAŞLATAN)
        // ===============================================
        for (auto it = powerUps.begin(); it != powerUps.end(); ) {
            // 1. Hareket: Kutuyu aşağı kaydır
            it->sprite.move({0.f, 200.f * dt});

            // 2. Yakalama Kontrolü (SFML 3.0 findIntersection)
            if (it->sprite.getGlobalBounds().findIntersection(paddle.getGlobalBounds())) {
                
                // --- EFEKTLERİ BURADA BAŞLATIYORUZ ---
                if (it->type == PowerUpType::ExpandPaddle) {
                    paddle.setScale({
                        ((float)PADDLE_WIDTH * 1.5f) / paddle.getLocalBounds().size.x,
                        (float)PADDLE_HEIGHT / paddle.getLocalBounds().size.y
                    });
                    paddleEffectEnd = currentTime + 7.f;
                } 
                else if (it->type == PowerUpType::SlowBall) {
                    float len = std::sqrt(ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y);
                    ballVelocity = (ballVelocity / len) * 150.f;
                    speedEffectEnd = currentTime + 5.f;
                } 
                else if (it->type == PowerUpType::SpeedBall) {
                    float len = std::sqrt(ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y);
                    ballVelocity = (ballVelocity / len) * 500.f;
                    speedEffectEnd = currentTime + 5.f;
                }

                it = powerUps.erase(it); // Yakalanınca sil
            } 
            else if (it->sprite.getPosition().y > WINDOW_HEIGHT) {
                it = powerUps.erase(it); // Ekrandan çıkınca sil
            } 
            else {
                ++it;
            }
        }  
        // ===============================================
        // RESET EFFECTS
        // ===============================================

        if (currentTime > paddleEffectEnd) {

            paddle.setScale({
                (float)PADDLE_WIDTH /
                paddle.getLocalBounds().size.x,

                (float)PADDLE_HEIGHT /
                paddle.getLocalBounds().size.y
            });
        }

        if (currentTime > speedEffectEnd) {

            float normalSpeed =
                std::min(
                    200.f + level * 15.f,
                    400.f
                );

            float currentSpeed =
                std::sqrt(
                    ballVelocity.x *
                    ballVelocity.x +

                    ballVelocity.y *
                    ballVelocity.y
                );

            if (
                std::abs(
                    currentSpeed -
                    normalSpeed
                ) > 10.f
            ) {

                ballVelocity =
                    (ballVelocity / currentSpeed)
                    * normalSpeed;
            }
        }

        // ===============================================
        // NEXT LEVEL(daha güvenli olmalı)
        // ===============================================

        if (bricks.empty()) {

            level++;

            createLevel(level);

            // Eğer createLevel (loadLevel) sonrası hala bricks boşsa, 
            // demek ki bir sonraki .txt dosyası yok.
            if (bricks.empty()) {
                std::cout << "TEBRIKLER! BUTUN LEVELLERI BITIRDIN!\n";
                window.close(); // Veya level = 1 diyerek başa döndürebilirsin
            } else {
                // Yeni level yüklendiyse topu sıfırla
                ball.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
                float speed = std::min(200.f + level * 15.f, 400.f);
                ballVelocity = {speed, -speed};
            }
        
        }

        // ===============================================
        // UI
        // ===============================================

        scoreText.setString(
            "SCORE: " +
            std::to_string(score) +

            "   LEVEL: " +
            std::to_string(level)
        );

        // ===============================================
        // DRAW
        // ===============================================

        window.clear(sf::Color::Black);

        window.draw(paddle);
        window.draw(ball);

        for (auto& brick : bricks)
            window.draw(brick.sprite);

        for (auto& p : powerUps)
            window.draw(p.sprite);

        window.draw(scoreText);

        window.display();
    }

    return 0;
}