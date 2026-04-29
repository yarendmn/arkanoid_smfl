#include <SFML/Graphics.hpp>
#include <vector>
#include "definitions.hpp"

int main() {
    // Pencere oluşturma (Definitions'tan gelen genişlik ve yükseklik ile)
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Arkanoid - Pro Build");
    window.setFramerateLimit(60);

    // Paddle (Çubuk) oluşturma
    sf::RectangleShape paddle(sf::Vector2f({PADDLE_WIDTH, PADDLE_HEIGHT}));
    paddle.setFillColor(sf::Color(121, 185, 142));
    paddle.setPosition({(WINDOW_WIDTH - PADDLE_WIDTH) / 2.f, WINDOW_HEIGHT - 50.f});

    // Top oluşturma
    sf::CircleShape ball(BALL_RADIUS);
    ball.setFillColor(sf::Color(185, 121, 142));
    ball.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
    sf::Vector2f ballVelocity({BALL_SPEED_X, BALL_SPEED_Y});

    // Tuğlaları oluşturma
    std::vector<sf::RectangleShape> bricks;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
            sf::RectangleShape brick(sf::Vector2f({BRICK_WIDTH, BRICK_HEIGHT}));
            brick.setFillColor(sf::Color(181, 12, 68));
            // Konum hesaplarken tanımladığımız sabitleri kullanıyoruz
            brick.setPosition({ 
                (float)i * (BRICK_WIDTH + BRICK_OFFSET) + 15.f, 
                (float)j * (BRICK_HEIGHT + BRICK_OFFSET) + 50.f 
            });
            bricks.push_back(brick);
        }
    }

    // Oyun Döngüsü
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }

        // Hareket Kontrolleri
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && paddle.getPosition().x > 0) 
            paddle.move({-PADDLE_SPEED, 0.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && paddle.getPosition().x < WINDOW_WIDTH - PADDLE_WIDTH) 
            paddle.move({PADDLE_SPEED, 0.f});

        ball.move(ballVelocity);

        // Pencere Sınırları ve Sekme Mantığı
        if (ball.getPosition().x < 0 || ball.getPosition().x > WINDOW_WIDTH - (BALL_RADIUS * 2)) 
            ballVelocity.x = -ballVelocity.x;
        if (ball.getPosition().y < 0) 
            ballVelocity.y = -ballVelocity.y;
        
        // Top aşağı düşerse resetle
        if (ball.getPosition().y > WINDOW_HEIGHT) 
            ball.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});

        // Paddle ile çarpışma
        if (ball.getGlobalBounds().findIntersection(paddle.getGlobalBounds())) 
            ballVelocity.y = - std::abs(ballVelocity.y);

        // Tuğlalarla çarpışma
        for (int i = 0; i < bricks.size(); ++i) {
            if (ball.getGlobalBounds().findIntersection(bricks[i].getGlobalBounds())) {
                ballVelocity.y = -ballVelocity.y;
                bricks.erase(bricks.begin() + i); // Çarpılan tuğlayı sil
                break; 
            }
        }

        // Çizim İşlemleri
        window.clear(sf::Color::Black);
        window.draw(paddle);
        window.draw(ball);
        for (auto& brick : bricks) window.draw(brick);
        window.display();
    }

    return 0;
}