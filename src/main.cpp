#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "definitions.hpp"
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

enum class PowerUpType { ExtendPaddle, SlowBall, SpeedBall };

struct GameBrick {
    sf::Sprite sprite;
    int hp;
    GameBrick(const sf::Texture& texture) : sprite(texture) { hp = 1; }
};

struct PowerUp {
    sf::Sprite sprite;
    PowerUpType type;
    PowerUp(const sf::Texture& texture, PowerUpType t) : sprite(texture), type(t) {}
};

int main() {
    srand(static_cast<unsigned int>(time(NULL)));
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Arkanoid - Power-Up Pro");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("assets/fonts/font.ttf")) {
        std::cout << "Hata: Font dosyasi bulunamadi!" << std::endl;
    }

    int score = 0;
    int level = 1;
    sf::Text scoreText(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({20.f, 10.f});

    sf::Texture paddleTexture, ballTexture, brickTexture, powerUpTexture;
    paddleTexture.loadFromFile("assets/textures/paddle.png");
    ballTexture.loadFromFile("assets/textures/ball.png");
    brickTexture.loadFromFile("assets/textures/brick.png");
    
    if (!powerUpTexture.loadFromFile("assets/textures/powerup.png")) {
        sf::Image img({20, 20}, sf::Color::Green);
        (void)powerUpTexture.loadFromImage(img);
    }

    sf::Sprite paddle(paddleTexture);
    paddle.setScale(sf::Vector2f((float)PADDLE_WIDTH / paddle.getLocalBounds().size.x, (float)PADDLE_HEIGHT / paddle.getLocalBounds().size.y));
    paddle.setPosition(sf::Vector2f((WINDOW_WIDTH - paddle.getGlobalBounds().size.x) / 2.f, WINDOW_HEIGHT - 50.f));

    sf::Sprite ball(ballTexture);
    ball.setPosition(sf::Vector2f{WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
    sf::Vector2f ballVelocity({BALL_SPEED_X, BALL_SPEED_Y});

    std::vector<GameBrick> bricks;
    std::vector<PowerUp> powerUps;
    
    sf::Clock gameClock;
    float paddleEffectEndTime = 0.f;
    float speedEffectEndTime = 0.f;

    auto createLevel = [&](int currentLevel) {
        bricks.clear();
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < (3 + currentLevel); ++j) {
                GameBrick brick(brickTexture);
                if (j < 2 && currentLevel > 1) {
                    brick.hp = 2;
                    brick.sprite.setColor(sf::Color(150, 150, 150));
                }
                brick.sprite.setScale(sf::Vector2f((float)BRICK_WIDTH / brick.sprite.getLocalBounds().size.x, (float)BRICK_HEIGHT / brick.sprite.getLocalBounds().size.y));
                brick.sprite.setPosition(sf::Vector2f((float)i * (BRICK_WIDTH + BRICK_OFFSET) + 15.f, (float)j * (BRICK_HEIGHT + BRICK_OFFSET) + 60.f));
                bricks.push_back(brick);
            }
        }
    };

    createLevel(level);

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }

        float currentTime = gameClock.getElapsedTime().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && paddle.getPosition().x > 0)
            paddle.move({-PADDLE_SPEED, 0.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && paddle.getPosition().x < WINDOW_WIDTH - paddle.getGlobalBounds().size.x)
            paddle.move({PADDLE_SPEED, 0.f});

        ball.move(ballVelocity);

        if (ball.getPosition().x < 0 || ball.getPosition().x > WINDOW_WIDTH - ball.getGlobalBounds().size.x)
            ballVelocity.x = -ballVelocity.x;
        if (ball.getPosition().y < 0)
            ballVelocity.y = -ballVelocity.y;
        if (ball.getPosition().y > WINDOW_HEIGHT) {
            ball.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
            float normalSpeed = std::min(200.f + (level * 15.f), 400.f);
            ballVelocity = {normalSpeed, -normalSpeed};
        }

        if (ball.getGlobalBounds().findIntersection(paddle.getGlobalBounds()))
            ballVelocity.y = -std::abs(ballVelocity.y);

        for (auto it = bricks.begin(); it != bricks.end(); ) {
            if (ball.getGlobalBounds().findIntersection(it->sprite.getGlobalBounds())) {
                ballVelocity.y = -ballVelocity.y;
                it->hp--;
                if (it->hp <= 0) {
                    if (rand() % 100 < 20) {
                        int rType = rand() % 3;
                        PowerUpType pType = (rType == 0) ? PowerUpType::ExtendPaddle : (rType == 1 ? PowerUpType::SlowBall : PowerUpType::SpeedBall);
                        PowerUp pu(powerUpTexture, pType);
                        pu.sprite.setColor(rType == 0 ? sf::Color::Green : (rType == 1 ? sf::Color::Cyan : sf::Color::Red));
                        pu.sprite.setPosition(sf::Vector2f(it->sprite.getPosition().x + (BRICK_WIDTH / 2.f) - 10.f, it->sprite.getPosition().y));
                        powerUps.push_back(pu);
                    }
                    it = bricks.erase(it);
                    score += 10;
                } else {
                    it->sprite.setColor(sf::Color::White);
                    it++;
                }
                break;
            } else it++;
        }

        for (auto it = powerUps.begin(); it != powerUps.end(); ) {
            it->sprite.move({0.f, 3.f});
            if (it->sprite.getGlobalBounds().findIntersection(paddle.getGlobalBounds())) {
                if (it->type == PowerUpType::ExtendPaddle) {
                    paddle.setScale(sf::Vector2f(((float)PADDLE_WIDTH * 1.5f) / paddle.getLocalBounds().size.x, (float)PADDLE_HEIGHT / paddle.getLocalBounds().size.y));
                    paddleEffectEndTime = currentTime + 7.0f;
                } else if (it->type == PowerUpType::SlowBall) {
                    ballVelocity = (ballVelocity / std::sqrt(ballVelocity.x*ballVelocity.x + ballVelocity.y*ballVelocity.y)) * 150.f;
                    speedEffectEndTime = currentTime + 5.0f;
                } else if (it->type == PowerUpType::SpeedBall) {
                    ballVelocity = (ballVelocity / std::sqrt(ballVelocity.x*ballVelocity.x + ballVelocity.y*ballVelocity.y)) * 500.f;
                    speedEffectEndTime = currentTime + 5.0f;
                }
                it = powerUps.erase(it);
            } else if (it->sprite.getPosition().y > WINDOW_HEIGHT) it = powerUps.erase(it);
            else it++;
        }

        if (currentTime > paddleEffectEndTime) {
            paddle.setScale(sf::Vector2f((float)PADDLE_WIDTH / paddle.getLocalBounds().size.x, (float)PADDLE_HEIGHT / paddle.getLocalBounds().size.y));
        }
        if (currentTime > speedEffectEndTime) {
            float normalSpeed = std::min(200.f + (level * 15.f), 400.f);
            float currentSpeed = std::sqrt(ballVelocity.x*ballVelocity.x + ballVelocity.y*ballVelocity.y);
            if (std::abs(currentSpeed - normalSpeed) > 10.f) ballVelocity = (ballVelocity / currentSpeed) * normalSpeed;
        }

        if (bricks.empty()) {
            level++;
            createLevel(level);
        }

        scoreText.setString("SCORE: " + std::to_string(score) + "  LEVEL: " + std::to_string(level));
        window.clear(sf::Color::Black);
        window.draw(paddle);
        window.draw(ball);
        for (auto& b : bricks) window.draw(b.sprite);
        for (auto& p : powerUps) window.draw(p.sprite);
        window.draw(scoreText);
        window.display();
    }
    return 0;
}