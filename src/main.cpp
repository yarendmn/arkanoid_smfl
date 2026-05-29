#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>

#include "definitions.hpp"
#include "level.hpp"
#include "powerup.hpp"

struct BallInstance {
    sf::Sprite  sprite;
    sf::Vector2f velocity;

    BallInstance(const sf::Texture& tex)
        : sprite(tex), velocity({0.f, 0.f}) {}
};

// ------------------------------------------------------------------
// Çarpışma tarafını belirleyip velocity'yi doğru eksende ters çevir
// ------------------------------------------------------------------
static void resolveBallBrickCollision(
    BallInstance& ball,
    const sf::FloatRect& brickBounds)
{
    sf::FloatRect ballBounds = ball.sprite.getGlobalBounds();

    float overlapLeft  = (ballBounds.position.x + ballBounds.size.x) - brickBounds.position.x;
    float overlapRight = (brickBounds.position.x + brickBounds.size.x) - ballBounds.position.x;
    float overlapTop   = (ballBounds.position.y + ballBounds.size.y) - brickBounds.position.y;
    float overlapBot   = (brickBounds.position.y + brickBounds.size.y) - ballBounds.position.y;

    bool fromLeft  = std::abs(overlapLeft)  < std::abs(overlapRight);
    bool fromTop   = std::abs(overlapTop)   < std::abs(overlapBot);

    float minX = fromLeft  ? overlapLeft  : overlapRight;
    float minY = fromTop   ? overlapTop   : overlapBot;

    if (std::abs(minX) < std::abs(minY)) {
        ball.velocity.x = fromLeft ? -std::abs(ball.velocity.x) : std::abs(ball.velocity.x);
    } else {
        ball.velocity.y = fromTop ? -std::abs(ball.velocity.y) : std::abs(ball.velocity.y);
    }
}

// ------------------------------------------------------------------
// Paddle çarpışması: paddle ortasına göre açı ver, top dışarı çık
// ------------------------------------------------------------------
static void resolveBallPaddleCollision(BallInstance& ball, const sf::Sprite& paddle)
{
    sf::FloatRect pb = paddle.getGlobalBounds();
    sf::FloatRect bb = ball.sprite.getGlobalBounds();

    if (ball.velocity.y < 0.f) return;

    float paddleCenterX = pb.position.x + pb.size.x / 2.f;
    float ballCenterX   = bb.position.x + bb.size.x / 2.f;
    float relativeHit   = (ballCenterX - paddleCenterX) / (pb.size.x / 2.f);

    float speed = std::sqrt(ball.velocity.x * ball.velocity.x + ball.velocity.y * ball.velocity.y);
    float angle = relativeHit * 60.f; 
    float rad   = angle * 3.14159265f / 180.f;

    ball.velocity.x =  speed * std::sin(rad);
    ball.velocity.y = -speed * std::cos(rad);

    ball.sprite.setPosition({
        bb.position.x,
        pb.position.y - bb.size.y - 0.5f
    });
}

// ------------------------------------------------------------------
int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    sf::RenderWindow window(
        sf::VideoMode({(unsigned)WINDOW_WIDTH, (unsigned)WINDOW_HEIGHT}),
        "YZM104 - Arkanoid Projesi");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("assets/fonts/font.ttf"))
        std::cout << "Font yuklenemedi!\n";

    sf::Texture paddleTexture, ballTexture, brickTexture, powerTexture;

    if (!paddleTexture.loadFromFile("assets/textures/paddle.png"))
        std::cout << "Paddle texture yuklenemedi!\n";

    if (!ballTexture.loadFromFile("assets/textures/ball.png"))
        std::cout << "Ball texture yuklenemedi!\n";

    if (!brickTexture.loadFromFile("assets/textures/brick.png"))
        std::cout << "Brick texture yuklenemedi!\n";

    if (!powerTexture.loadFromFile("assets/textures/powerup.png")) {
        sf::Image img({20, 20}, sf::Color::Green);
        powerTexture.loadFromImage(img);
    }

    int score = 0;
    int level = 1;
    int lives = 3;

    sf::Text scoreText(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({20.f, 10.f});

    sf::Text transitionText(font);
    transitionText.setCharacterSize(40);
    transitionText.setFillColor(sf::Color::Yellow);

    sf::Sprite paddle(paddleTexture);
    paddle.setScale({
        (float)PADDLE_WIDTH  / paddle.getLocalBounds().size.x,
        (float)PADDLE_HEIGHT / paddle.getLocalBounds().size.y
    });
    paddle.setPosition({
        (WINDOW_WIDTH  - paddle.getGlobalBounds().size.x) / 2.f,
        WINDOW_HEIGHT - 50.f
    });

    auto makeBall = [&]() -> BallInstance {
        BallInstance b(ballTexture);
        b.sprite.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
        b.velocity = {250.f, -250.f};
        return b;
    };

    std::vector<BallInstance> balls;
    balls.push_back(makeBall());

    std::vector<GameBrick> bricks;
    auto createLevel = [&](int currentLevel) {
        bricks.clear();
        loadLevel(currentLevel, bricks, brickTexture);
    };
    createLevel(level);

    std::vector<PowerUp> powerUps;

    float paddleEffectEnd = 0.f; 
    float speedEffectEnd  = 0.f; 
    bool  paddleWidened   = false;
    bool  speedBoosted    = false;

    bool  isTransitioning  = false;
    float transitionEndTime = 0.f;
    bool  isGameOver = false;
    bool  isWin      = false;

    sf::Clock deltaClock;
    sf::Clock gameClock;

    while (window.isOpen()) {

        float dt          = deltaClock.restart().asSeconds();
        float currentTime = gameClock.getElapsedTime().asSeconds();

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        if (paddleWidened && currentTime > paddleEffectEnd) {
            paddle.setScale({
                (float)PADDLE_WIDTH / paddleTexture.getSize().x,
                (float)PADDLE_HEIGHT / paddleTexture.getSize().y
            });
            paddleWidened = false;
        }

        if (speedBoosted && currentTime > speedEffectEnd) {
            for (auto& b : balls) {
                float spd = std::sqrt(b.velocity.x * b.velocity.x + b.velocity.y * b.velocity.y);
                if (spd > 0.f) {
                    float scale = 250.f / spd; // Normal hıza sabitle
                    b.velocity.x *= scale;
                    b.velocity.y *= scale;
                }
            }
            speedBoosted = false;
        }

        if (bricks.empty() && !isTransitioning && !isWin) {
            isTransitioning  = true;
            transitionEndTime = currentTime + 3.f;
            transitionText.setString("BOLUM TAMAMLANDI!");
            transitionText.setPosition({
                (WINDOW_WIDTH  - transitionText.getGlobalBounds().size.x) / 2.f,
                (WINDOW_HEIGHT - transitionText.getGlobalBounds().size.y) / 2.f
            });
        }

        if (isTransitioning) {
            if (currentTime > transitionEndTime) {
                level++;
                createLevel(level);

                if (bricks.empty()) {
                    isWin = true;
                } else {
                    balls.clear();
                    powerUps.clear();
                    balls.push_back(makeBall());
                    isTransitioning = false;
                }
            }
        }
        else if (!isGameOver && !isWin) {

            float paddleLeft  = paddle.getPosition().x;
            float paddleRight = paddleLeft + paddle.getGlobalBounds().size.x;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && paddleLeft > 0.f)
                paddle.move({-PADDLE_SPEED * dt, 0.f});

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && paddleRight < (float)WINDOW_WIDTH)
                paddle.move({ PADDLE_SPEED * dt, 0.f});

            std::vector<BallInstance> newBalls; 

            for (auto ballIt = balls.begin(); ballIt != balls.end(); ) {

                ballIt->sprite.move(ballIt->velocity * dt);
                auto pos  = ballIt->sprite.getPosition();
                auto size = ballIt->sprite.getGlobalBounds().size;

                if (pos.x <= 0.f) ballIt->velocity.x = std::abs(ballIt->velocity.x);
                if (pos.x + size.x >= (float)WINDOW_WIDTH) ballIt->velocity.x = -std::abs(ballIt->velocity.x);
                if (pos.y <= 0.f) ballIt->velocity.y = std::abs(ballIt->velocity.y);

                if (pos.y > (float)WINDOW_HEIGHT) {
                    ballIt = balls.erase(ballIt);
                    continue;
                }

                if (ballIt->sprite.getGlobalBounds().findIntersection(paddle.getGlobalBounds()).has_value()) {
                    resolveBallPaddleCollision(*ballIt, paddle);
                }

                for (auto it = bricks.begin(); it != bricks.end(); ) {
                    if (ballIt->sprite.getGlobalBounds().findIntersection(it->sprite.getGlobalBounds()).has_value()) {
                        resolveBallBrickCollision(*ballIt, it->sprite.getGlobalBounds());
                        it->hp--;

                        if (it->hp <= 0) {
                            spawnPowerUp(powerUps, powerTexture, it->sprite.getPosition());
                            it = bricks.erase(it);
                            score += 10;
                        } else {
                            sf::Color currentColor = it->sprite.getColor();
                            currentColor.a = 120; 
                            it->sprite.setColor(currentColor);
                            ++it;
                        }
                        break; 
                    } else {
                        ++it;
                    }
                }
                ++ballIt;
            }

            for (auto& b : newBalls) balls.push_back(b);

            // -- PowerUp Yönetimi (Senin tanımladığın 4 özelliğe tam uyumlu) --
            for (auto it = powerUps.begin(); it != powerUps.end(); ) {

                it->sprite.move(sf::Vector2f(0.f, 200.f * dt));

                if (it->sprite.getGlobalBounds().findIntersection(paddle.getGlobalBounds()).has_value()) {
                    
                    switch (it->type) {
                        case PowerUpType::MultiBall:
                            for (auto& b : balls) {
                                BallInstance nb(ballTexture);
                                nb.sprite.setPosition(b.sprite.getPosition());
                                nb.velocity = { -b.velocity.y, b.velocity.x };
                                newBalls.push_back(nb);
                            }
                            break;

                        case PowerUpType::ExpandPaddle:
                            paddle.setScale({
                                (float)(PADDLE_WIDTH * 1.5f) / paddleTexture.getSize().x,
                                (float)PADDLE_HEIGHT / paddleTexture.getSize().y
                            });
                            paddleWidened    = true;
                            paddleEffectEnd  = currentTime + 8.f;
                            break;

                        case PowerUpType::SlowBall: // Turuncu Güçlendirici
                            if (!speedBoosted) {
                                for (auto& b : balls) {
                                    b.velocity.x *= 0.6f;
                                    b.velocity.y *= 0.6f;
                                }
                                speedBoosted   = true;
                                speedEffectEnd = currentTime + 6.f;
                            }
                            break;
                            
                        case PowerUpType::SpeedBall: // Magenta Güçlendirici (YENİ EKLENDİ)
                            if (!speedBoosted) {
                                for (auto& b : balls) {
                                    b.velocity.x *= 1.5f;
                                    b.velocity.y *= 1.5f;
                                }
                                speedBoosted   = true;
                                speedEffectEnd = currentTime + 6.f;
                            }
                            break;

                        default:
                            break;
                    }
                    it = powerUps.erase(it);
                }
                else if (it->sprite.getPosition().y > (float)WINDOW_HEIGHT) {
                    it = powerUps.erase(it);
                } else {
                    ++it;
                }
            }

            if (balls.empty() && !bricks.empty()) {
                lives--;
                if (lives <= 0) {
                    isGameOver = true;
                } else {
                    powerUps.clear();
                    balls.push_back(makeBall());
                }
            }
        }

        scoreText.setString(
            "SKOR: " + std::to_string(score) +
            "   SEVIYE: " + std::to_string(level) +
            "   CAN: " + std::to_string(lives));

        window.clear(sf::Color::Black);

        if (isGameOver || isWin) {
            sf::Text endText(font);
            endText.setCharacterSize(50);

            if (isWin) {
                endText.setString("TEBRIKLER!\nOYUNU KAZANDIN!"); 
                endText.setFillColor(sf::Color::Green);
            } else {
                endText.setString("OYUN BITTI!"); 
                endText.setFillColor(sf::Color::Red);
            }

            endText.setPosition({
                (WINDOW_WIDTH  - endText.getGlobalBounds().size.x) / 2.f,
                (WINDOW_HEIGHT - endText.getGlobalBounds().size.y) / 2.f
            });

            window.draw(endText);
            window.draw(scoreText);

        } else {
            for (auto& brick : bricks) window.draw(brick.sprite);
            for (auto& p     : powerUps) window.draw(p.sprite);
            for (auto& b     : balls)   window.draw(b.sprite);
            window.draw(paddle);
            window.draw(scoreText);

            if (isTransitioning) window.draw(transitionText);
        }

        window.display();
    }

    return 0;
}