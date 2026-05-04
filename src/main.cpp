#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "definitions.hpp"

int main() {
    // Pencere oluşturma
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Arkanoid - Pro Build");
    window.setFramerateLimit(60);

    // --- GÖRSEL ÖĞELER VE YEDEK MEKANİZMASI ---
    
    // Paddle Dokusu
    sf::Texture paddleTexture;
    if (!paddleTexture.loadFromFile("assets/textures/paddle.png")) {
        std::cout << "Uyari: paddle.png bulunamadi, yedek renk yukleniyor." << std::endl;
        // SFML 3.0'da create yerine constructor kullanıyoruz
        sf::Image img({(unsigned int)PADDLE_WIDTH, (unsigned int)PADDLE_HEIGHT}, sf::Color(121, 185, 142));
        (void)paddleTexture.loadFromImage(img);
    }
    
    // Top Dokusu
    sf::Texture ballTexture;
    sf::Image ballImage;
    if (!ballTexture.loadFromFile("assets/textures/ball.png")) {
        ballImage.createMaskFromColor(sf::Color::White); 
        (void)ballTexture.loadFromImage(ballImage);

    }else{
        std::cout << "Uyari: ball.png bulunamadi, yedek renk yukleniyor." << std::endl;

        unsigned int diameter = (unsigned int)(BALL_RADIUS * 2);
        sf::Image img({diameter, diameter}, sf::Color::Transparent);

        for(unsigned int x = 0; x < diameter; ++x) {
            for(unsigned int y = 0; y < diameter; ++y) {
                // merkezden uzaklığı kontrol ederek daire oluşturuyoruz
                float dx = x - BALL_RADIUS;
                float dy = y - BALL_RADIUS;
                // Eğer merkeze olan uzaklık yarıçaptan küçükse, o pikseli boya
                if (dx * dx + dy * dy <= BALL_RADIUS * BALL_RADIUS) {
                    img.setPixel(sf::Vector2u{x,y}, sf::Color(185, 121, 142));
                }
            }
        }
        (void)ballTexture.loadFromImage(img);
    }

    // Tuğla Dokusu
    sf::Texture brickTexture;
    if (!brickTexture.loadFromFile("assets/textures/brick.png")) {
        std::cout << "Uyari: brick.png bulunamadi, yedek renk yukleniyor." << std::endl;
        sf::Image img({(unsigned int)BRICK_WIDTH, (unsigned int)BRICK_HEIGHT}, sf::Color(181, 12, 68));
        (void)brickTexture.loadFromImage(img);
    }

    // Font Yükleme (SFML 3.0'da openFromFile kullanılır)
    sf::Font font;
    (void)font.openFromFile("assets/fonts/font.ttf");

    // --- OBJELERİN OLUŞTURULMASI ---

    // SFML 3.0'da Sprite'a texture'ı constructor'da vermeliyiz
    sf::Sprite paddle(paddleTexture);
    paddle.setPosition({(WINDOW_WIDTH - PADDLE_WIDTH) / 2.f, WINDOW_HEIGHT - 50.f});

    sf::Sprite ball(ballTexture);
    ball.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
    sf::Vector2f ballVelocity({BALL_SPEED_X, BALL_SPEED_Y});

    std::vector<sf::Sprite> bricks;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 4; ++j) {
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

    // --- OYUN DÖNGÜSÜ ---
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
        if (ball.getPosition().x < 0 || ball.getPosition().x > WINDOW_WIDTH - ball.getGlobalBounds().size.x) 
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
        for (int i = 0; i < (int)bricks.size(); ++i) {
            if (ball.getGlobalBounds().findIntersection(bricks[i].getGlobalBounds())) {
                ballVelocity.y = -ballVelocity.y;
                bricks.erase(bricks.begin() + i);
                break; 
            }
        }

        // --- ÇİZİM İŞLEMLERİ ---
        window.clear(sf::Color::Black);
        window.draw(paddle);
        window.draw(ball);
        for (auto& brick : bricks) window.draw(brick);
        window.display();
    }

    return 0;
}