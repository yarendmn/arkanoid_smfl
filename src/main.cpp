#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <fstream> // Dosya işlemleri için
#include "definitions.hpp"
#include "level.hpp"
#include "powerup.hpp"


int main() {

    srand(static_cast<unsigned>(time(nullptr)));

    sf::RenderWindow window(
        sf::VideoMode({(unsigned)WINDOW_WIDTH, (unsigned)WINDOW_HEIGHT}),
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
    // SCORE & LEVEL & LIVES
    // ===================================================
    int score = 0;
    int level = 1;
    int lives = 3;

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

    if (!paddleTexture.loadFromFile("assets/textures/paddle.png")) {
        std::cout << "HATA: Paddle resmi bulunamadi!\n";
    }

    if (!ballTexture.loadFromFile("assets/textures/ball.png")) {
        std::cout << "HATA: Top resmi bulunamadi!\n";
    }

    if (!brickTexture.loadFromFile("assets/textures/brick.png")) {
        std::cout << "HATA: Tugla resmi bulunamadi!\n";
    }

    if (!powerTexture.loadFromFile("assets/textures/powerup.png")) {
        sf::Image img({20, 20}, sf::Color::Green);

        if (!powerTexture.loadFromImage(img)) {
            std::cout << "Powerup texture olusturulamadi!\n";
        }
    }

    // ===================================================
    // PADDLE
    // ===================================================
    sf::Sprite paddle(paddleTexture);

    paddle.setScale({
        (float)PADDLE_WIDTH / paddle.getLocalBounds().size.x,
        (float)PADDLE_HEIGHT / paddle.getLocalBounds().size.y
    });

    paddle.setPosition({
        (WINDOW_WIDTH - paddle.getGlobalBounds().size.x) / 2.f,
        WINDOW_HEIGHT - 50.f
    });

    // ===================================================
    // BALL SYSTEM
    // ===================================================
    struct BallInstance {

        sf::Sprite sprite;
        sf::Vector2f velocity;

        BallInstance(const sf::Texture& tex)
            : sprite(tex), velocity({0.f, 0.f}) {}
    };

    std::vector<BallInstance> balls;

    BallInstance mainBall(ballTexture);

    mainBall.sprite.setPosition({
        WINDOW_WIDTH / 2.f,
        WINDOW_HEIGHT / 2.f
    });

    mainBall.velocity = {
        BALL_SPEED_X,
        BALL_SPEED_Y
    };

    balls.push_back(mainBall);

    // ===================================================
    // LEVEL & BRICKS
    // ===================================================
    std::vector<GameBrick> bricks;

    auto createLevel = [&](int currentLevel) {

        bricks.clear();
        loadLevel(currentLevel, bricks, brickTexture);
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
    // GAME STATE & TRANSITION 
    // ===================================================
    bool isTransitioning = false;
    float transitionEndTime = 0.f;
    
    bool isGameOver = false; // <--- YENİ EKLENDİ
    bool isWin = false;      // <--- YENİ EKLENDİ

    sf::Text transitionText(font);

    transitionText.setCharacterSize(40);
    transitionText.setFillColor(sf::Color::Yellow);

    // ===================================================
    // GAME LOOP
    // ===================================================
    while (window.isOpen()) {

        float dt = deltaClock.restart().asSeconds();

        float currentTime =
            gameClock.getElapsedTime().asSeconds();

        // ===============================================
        // EVENTS
        // ===============================================
        while (const auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // ===============================================
        // LEVEL COMPLETE
        // ===============================================
        if (bricks.empty() && !isTransitioning) {

            isTransitioning = true;

            transitionEndTime = currentTime + 3.f;

            transitionText.setString(
                "LEVEL " + std::to_string(level) +
                " TAMAMLANDI!\n\nSONRAKI LEVEL " +
                std::to_string(level + 1)
            );

            transitionText.setPosition({
                (WINDOW_WIDTH -
                transitionText.getGlobalBounds().size.x) / 2.f,

                (WINDOW_HEIGHT -
                transitionText.getGlobalBounds().size.y) / 2.f
            });
        }

        // ===============================================
        // TRANSITION
        // ===============================================
        if (isTransitioning) {

            if (currentTime > transitionEndTime) {

                level++;

                createLevel(level);

                if (bricks.empty()) {

                    std::cout
                        << "TEBRIKLER! BUTUN LEVELLERI BITIRDIN!\n";

                    isWin = true; // <--- DEĞİŞTİRİLDİ (window.close() sildik)
                }
                else {

                    balls.clear();

                    BallInstance newMainBall(ballTexture);

                    newMainBall.sprite.setPosition({
                        WINDOW_WIDTH / 2.f,
                        WINDOW_HEIGHT / 2.f
                    });

                    float speed =
                        std::min(200.f + level * 15.f, 400.f);

                    newMainBall.velocity = {
                        speed,
                        -speed
                    };

                    balls.push_back(newMainBall);

                    paddle.setPosition({
                        (WINDOW_WIDTH -
                        paddle.getGlobalBounds().size.x) / 2.f,

                        WINDOW_HEIGHT - 50.f
                    });

                    isTransitioning = false;
                }
            }
        }
        else if (!isGameOver && !isWin) { // <--- DEĞİŞTİRİLDİ (Oyun bittiyse her şeyi dondurur)

            // ===========================================
            // PADDLE MOVEMENT
            // ===========================================
            if (
                sf::Keyboard::isKeyPressed(
                    sf::Keyboard::Key::Left
                )
                &&
                paddle.getPosition().x > 0
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
                &&
                paddle.getPosition().x <
                WINDOW_WIDTH -
                paddle.getGlobalBounds().size.x
            ) {

                paddle.move({
                    PADDLE_SPEED * dt,
                    0.f
                });
            }

            // ===========================================
            // BALLS
            // ===========================================
            for (
                auto ballIt = balls.begin();
                ballIt != balls.end();
            ) {

                // MOVE
                ballIt->sprite.move(
                    ballIt->velocity * dt
                );

                // LEFT WALL
                if (ballIt->sprite.getPosition().x <= 0) {

                    ballIt->velocity.x =
                        std::abs(ballIt->velocity.x);

                    ballIt->sprite.setPosition({
                        0.f,
                        ballIt->sprite.getPosition().y
                    });
                }

                // RIGHT WALL
                if (
                    ballIt->sprite.getPosition().x >=
                    WINDOW_WIDTH -
                    ballIt->sprite.getGlobalBounds().size.x
                ) {

                    ballIt->velocity.x =
                        -std::abs(ballIt->velocity.x);

                    ballIt->sprite.setPosition({
                        WINDOW_WIDTH -
                        ballIt->sprite.getGlobalBounds().size.x,

                        ballIt->sprite.getPosition().y
                    });
                }

                // TOP WALL
                if (ballIt->sprite.getPosition().y <= 0) {

                    ballIt->velocity.y =
                        std::abs(ballIt->velocity.y);

                    ballIt->sprite.setPosition({
                        ballIt->sprite.getPosition().x,
                        0.f
                    });
                }

                // FALL DOWN
                if (
                    ballIt->sprite.getPosition().y >
                    WINDOW_HEIGHT
                ) {

                    ballIt = balls.erase(ballIt);
                    continue;
                }

                // PADDLE COLLISION
                if (
                    ballIt->sprite
                    .getGlobalBounds()
                    .findIntersection(
                        paddle.getGlobalBounds()
                    )
                ) {

                    ballIt->velocity.y =
                        -std::abs(ballIt->velocity.y);

                    ballIt->sprite.setPosition({

                        ballIt->sprite.getPosition().x,

                        paddle.getPosition().y -
                        ballIt->sprite.getGlobalBounds().size.y -
                        1.f
                    });

                    float paddleCenter =
                        paddle.getPosition().x +
                        paddle.getGlobalBounds().size.x / 2.f;

                    float ballCenter =
                        ballIt->sprite.getPosition().x +
                        ballIt->sprite.getGlobalBounds().size.x / 2.f;

                    float distance =
                        ballCenter - paddleCenter;

                    ballIt->velocity.x =
                        distance * 5.f;
                }

                // BRICKS
                for (
                    auto it = bricks.begin();
                    it != bricks.end();
                ) {

                    if (
                        ballIt->sprite
                        .getGlobalBounds()
                        .findIntersection(
                            it->sprite.getGlobalBounds()
                        )
                    ) {

                        ballIt->velocity.y =
                            -ballIt->velocity.y;

                        it->hp--;

                        if (it->hp <= 0) {

                            spawnPowerUp(
                                powerUps,
                                powerTexture,
                                it->sprite.getPosition()
                            );

                            it = bricks.erase(it);

                            score += 10;
                        }
                        else {

                            it->sprite.setColor(
                                sf::Color::White
                            );

                            ++it;
                        }

                        break;
                    }
                    else {
                        ++it;
                    }
                }

                ++ballIt;
            }

            // ===========================================
            // NO BALLS (CAN SİSTEMİ)
            // ===========================================
            if (
                balls.empty()
                &&
                !bricks.empty()
                &&
                !isTransitioning
            ) {

                lives--;

                if (lives > 0) {

                    BallInstance newBall(ballTexture);

                    newBall.sprite.setPosition({
                        WINDOW_WIDTH / 2.f,
                        WINDOW_HEIGHT / 2.f
                    });

                    float speed =
                        std::min(
                            200.f + level * 15.f,
                            400.f
                        );

                    newBall.velocity = {
                        speed,
                        -speed
                    };

                    balls.push_back(newBall);

                    paddle.setPosition({
                        (WINDOW_WIDTH -
                        paddle.getGlobalBounds().size.x) / 2.f,

                        WINDOW_HEIGHT - 50.f
                    });
                }
                else {

                    std::cout
                        << "OYUN BITTI! SKORUNUZ: "
                        << score << "\n";

                    isGameOver = true; // <--- DEĞİŞTİRİLDİ (window.close() sildik)
                }
            }

            // ===========================================
            // POWERUPS
            // ===========================================
            for (
                auto it = powerUps.begin();
                it != powerUps.end();
            ) {

                it->sprite.move({
                    0.f,
                    200.f * dt
                });

                if (
                    it->sprite
                    .getGlobalBounds()
                    .findIntersection(
                        paddle.getGlobalBounds()
                    )
                ) {

                    if (
                        it->type ==
                        PowerUpType::ExpandPaddle
                    ) {

                        paddle.setScale({

                            ((float)PADDLE_WIDTH * 1.5f)
                            /
                            paddle.getLocalBounds().size.x,

                            (float)PADDLE_HEIGHT
                            /
                            paddle.getLocalBounds().size.y
                        });

                        paddleEffectEnd =
                            currentTime + 7.f;
                    }

                    else if (
                        it->type ==
                        PowerUpType::SlowBall
                    ) {

                        for (auto& b : balls) {

                            float len = std::sqrt(
                                b.velocity.x * b.velocity.x +
                                b.velocity.y * b.velocity.y
                            );

                            b.velocity =
                                (b.velocity / len) * 150.f;
                        }

                        speedEffectEnd =
                            currentTime + 5.f;
                    }

                    else if (
                        it->type ==
                        PowerUpType::SpeedBall
                    ) {

                        for (auto& b : balls) {

                            float len = std::sqrt(
                                b.velocity.x * b.velocity.x +
                                b.velocity.y * b.velocity.y
                            );

                            b.velocity =
                                (b.velocity / len) * 500.f;
                        }

                        speedEffectEnd =
                            currentTime + 5.f;
                    }

                    else if (
                        it->type ==
                        PowerUpType::MultiBall
                    ) {

                        if (!balls.empty()) {

                            sf::Vector2f currentPos =
                                balls[0].sprite.getPosition();

                            sf::Vector2f currentVel =
                                balls[0].velocity;

                            BallInstance b2(ballTexture);

                            b2.sprite.setPosition(currentPos);

                            b2.velocity = {
                                -currentVel.x,
                                currentVel.y
                            };

                            BallInstance b3(ballTexture);

                            b3.sprite.setPosition(currentPos);

                            b3.velocity = {
                                currentVel.x * 0.5f,
                                currentVel.y * 1.2f
                            };

                            balls.push_back(b2);
                            balls.push_back(b3);
                        }
                    }

                    it = powerUps.erase(it);
                }

                else if (
                    it->sprite.getPosition().y >
                    WINDOW_HEIGHT
                ) {

                    it = powerUps.erase(it);
                }

                else {
                    ++it;
                }
            }

            // ===========================================
            // RESET EFFECTS
            // ===========================================
            if (currentTime > paddleEffectEnd) {

                paddle.setScale({

                    (float)PADDLE_WIDTH
                    /
                    paddle.getLocalBounds().size.x,

                    (float)PADDLE_HEIGHT
                    /
                    paddle.getLocalBounds().size.y
                });
            }

            if (currentTime > speedEffectEnd) {

                float normalSpeed =
                    std::min(
                        200.f + level * 15.f,
                        400.f
                    );

                for (auto& b : balls) {

                    float currentSpeed = std::sqrt(
                        b.velocity.x * b.velocity.x +
                        b.velocity.y * b.velocity.y
                    );

                    if (
                        std::abs(currentSpeed - normalSpeed)
                        > 10.f
                    ) {

                        b.velocity =
                            (b.velocity / currentSpeed)
                            * normalSpeed;
                    }
                }
            }
        }

        // ===============================================
        // UI
        // ===============================================
        scoreText.setString(
            "SCORE: " + std::to_string(score)
            +
            "   LEVEL: " + std::to_string(level)
            +
            "   LIVES: " + std::to_string(lives)
        );

        // ===============================================
        // DRAW
        // ===============================================
        window.clear(sf::Color::Black);

        // ===============================================
        // OYUN BİTİŞ VEYA KAZANMA EKRANI (YENİ EKLENDİ)
        // ===============================================
        if (isGameOver || isWin) {
            
            sf::Text endText(font);
            endText.setCharacterSize(50);
            
            if (isWin) {
                endText.setString("TEBRIKLER! OYUNU KAZANDIN!\nFINAL SKOR: " + std::to_string(score));
                endText.setFillColor(sf::Color::Green);
            } else {
                endText.setString("GAME OVER!\nFINAL SKOR: " + std::to_string(score));
                endText.setFillColor(sf::Color::Red);
            }

            // Yazıyı ekranın tam ortasına hizala
            endText.setPosition({
                (WINDOW_WIDTH - endText.getGlobalBounds().size.x) / 2.f,
                (WINDOW_HEIGHT - endText.getGlobalBounds().size.y) / 2.f
            });

            window.draw(endText);
        }
        else {
            // Ekrana Oyun Objelerini Çiz (Oyun bitmediyse)
            window.draw(paddle);

            for (auto& b : balls)
                window.draw(b.sprite);

            for (auto& brick : bricks)
                window.draw(brick.sprite);

            for (auto& p : powerUps)
                window.draw(p.sprite);

            if (isTransitioning) {
                window.draw(transitionText);
            }

            window.draw(scoreText);
        }

        window.display();
    }

    return 0;
}