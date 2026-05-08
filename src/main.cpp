#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "definitions.hpp"
#include <string>

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Arkanoid - Pro Build");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("assets/fonts/font.ttf")) {
        std::cout << "Hata: Font dosyasi bulunamadi!" << std::endl;
    }

    int score = 0;
    int level = 1;
    sf::Text scoreText(font);
    scoreText.setString("SCORE: " + std::to_string(score) + "  LEVEL: " + std::to_string(level));
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({20.f, 10.f});

    sf::Texture paddleTexture;
    if (!paddleTexture.loadFromFile("assets/textures/paddle.png")) {
        sf::Image img({(unsigned int)PADDLE_WIDTH, (unsigned int)PADDLE_HEIGHT}, sf::Color(121, 185, 142));
        (void)paddleTexture.loadFromImage(img);
    }

    sf::Texture ballTexture;
    sf::Image ballImage;
    if (!ballTexture.loadFromFile("assets/textures/ball.png")) {
        ballImage.createMaskFromColor(sf::Color::White);
        (void)ballTexture.loadFromImage(ballImage);
    } else {
        unsigned int diameter = (unsigned int)(BALL_RADIUS * 2);
        sf::Image img({diameter, diameter}, sf::Color::Transparent);
        for (unsigned int x = 0; x < diameter; ++x) {
            for (unsigned int y = 0; y < diameter; ++y) {
                float dx = x - BALL_RADIUS;
                float dy = y - BALL_RADIUS;
                if (dx * dx + dy * dy <= BALL_RADIUS * BALL_RADIUS) {
                    img.setPixel(sf::Vector2u{x, y}, sf::Color(185, 121, 142));
                }
            }
        }
        (void)ballTexture.loadFromImage(img);
    }

    sf::Texture brickTexture;
    if (!brickTexture.loadFromFile("assets/textures/brick.png")) {
        sf::Image img({(unsigned int)BRICK_WIDTH, (unsigned int)BRICK_HEIGHT}, sf::Color(181, 12, 68));
        (void)brickTexture.loadFromImage(img);
    }

    sf::Sprite paddle(paddleTexture);
    paddle.setPosition({(WINDOW_WIDTH - PADDLE_WIDTH) / 2.f, WINDOW_HEIGHT - 50.f});

    sf::Sprite ball(ballTexture);
    ball.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
    sf::Vector2f ballVelocity({BALL_SPEED_X, BALL_SPEED_Y});

    std::vector<sf::Sprite> bricks;
    bool levelCleared = false; // ← BURASI KRİTİK, DÖNGÜNÜN DIŞINDA

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 1; ++j) {
            sf::Sprite brick(brickTexture);
            brick.setScale({(float)BRICK_WIDTH / brick.getLocalBounds().size.x,
                            (float)BRICK_HEIGHT / brick.getLocalBounds().size.y});
            brick.setPosition({
                (float)i * (BRICK_WIDTH + BRICK_OFFSET) + 15.f,
                (float)j * (BRICK_HEIGHT + BRICK_OFFSET) + 50.f
            });
            bricks.push_back(brick);
        }
    }

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && paddle.getPosition().x > 0)
            paddle.move({-PADDLE_SPEED, 0.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && paddle.getPosition().x < WINDOW_WIDTH - PADDLE_WIDTH)
            paddle.move({PADDLE_SPEED, 0.f});

        ball.move(ballVelocity);

        if (ball.getPosition().x < 0 || ball.getPosition().x > WINDOW_WIDTH - ball.getGlobalBounds().size.x)
            ballVelocity.x = -ballVelocity.x;
        if (ball.getPosition().y < 0)
            ballVelocity.y = -ballVelocity.y;

        if (ball.getPosition().y > WINDOW_HEIGHT){
            ball.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
            ballVelocity = {BALL_SPEED_X, -BALL_SPEED_Y};
        }
           

        if (ball.getGlobalBounds().findIntersection(paddle.getGlobalBounds()))
            ballVelocity.y = -std::abs(ballVelocity.y);

        for (int i = 0; i < (int)bricks.size(); ++i) {
            if (ball.getGlobalBounds().findIntersection(bricks[i].getGlobalBounds())) {
                ballVelocity.y = -ballVelocity.y;
                bricks.erase(bricks.begin() + i);
                score += 10;
                scoreText.setString("SCORE: " + std::to_string(score) + "  LEVEL: " + std::to_string(level));
                if (score % 50 == 0) {
                    paddle.setScale({paddle.getScale().x + 0.1f, 1.f});
                }
                break;
            }
        }

if (bricks.empty() && !levelCleared) {
    levelCleared = true;
    level++;

    ball.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
    float speed = std::min(200.f + (level * 15.f), 400.f);
    ballVelocity = {speed, -speed};

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < (3 + level); ++j) {
            sf::Sprite newBrick(brickTexture);

            newBrick.setScale({
                (float)BRICK_WIDTH / newBrick.getLocalBounds().size.x,
                (float)BRICK_HEIGHT / newBrick.getLocalBounds().size.y
            });

            newBrick.setPosition({
                (float)i * (BRICK_WIDTH + BRICK_OFFSET) + 15.f,
                (float)j * (BRICK_HEIGHT + BRICK_OFFSET) + 60.f
            });

            bricks.push_back(newBrick);
        }
    }

    scoreText.setString(
        "SCORE: " + std::to_string(score) +
        "  LEVEL: " + std::to_string(level)
    );

    levelCleared = false; // BURAYA
}

        window.clear(sf::Color::Black);
        window.draw(paddle);
        window.draw(ball);
        for (auto& brick : bricks) window.draw(brick);
        window.draw(scoreText);
        window.display();
    }

    return 0;
}