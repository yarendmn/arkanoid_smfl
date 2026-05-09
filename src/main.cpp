#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "definitions.hpp"
#include <string>
#include <algorithm>
#include <cstdlib> // rand() için
#include <ctime>   // time() için

struct GameBrick {
    sf::Sprite sprite;
    int hp;

    GameBrick(const sf::Texture& texture) : sprite(texture) {
        hp = 1;
    }
};

// YENİ: Power-Up Yapımız
struct PowerUp { // şuanlık tek tik power-up var, ama ileride çeşitlendirebiliriz
    sf::Sprite sprite;
    
    PowerUp(const sf::Texture& texture) : sprite(texture) {}
};

int main() {
    srand(time(NULL)); // Rastgele sayı üretecini başlat

    sf::RenderWindow window(sf::VideoMode({(unsigned int)WINDOW_WIDTH, (unsigned int)WINDOW_HEIGHT}), "Arkanoid - Pro Build");// oyun penceresi her şey buraya çizilir
    window.setFramerateLimit(60); // FPS'i 60 ile sınırlayarak daha stabil bir oyun deneyimi sağlar CPU tasarrufu için

    sf::Font font;
    if (!font.openFromFile("assets/fonts/font.ttf")) {
        std::cout << "Hata: Font dosyasi bulunamadi!" << std::endl;
    }

    int score = 0;
    int level = 1;
    sf::Text scoreText(font); // Skor ve seviye bilgisini göstermek için bir metin nesnesi oluşturulur
    scoreText.setString("SCORE: " + std::to_string(score) + "  LEVEL: " + std::to_string(level));
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({20.f, 10.f});

    sf::Texture paddleTexture;// bellekte rek dolu bir sf::Image olustur sonra texture yap
    if (!paddleTexture.loadFromFile("assets/textures/paddle.png")) {
        sf::Image img({(unsigned int)PADDLE_WIDTH, (unsigned int)PADDLE_HEIGHT}, sf::Color(121, 185, 142));
        (void)paddleTexture.loadFromImage(img);
    }

    sf::Texture ballTexture;
    sf::Image ballImage;
    if (!ballTexture.loadFromFile("assets/textures/ball.png")) {
        ballImage.createMaskFromColor(sf::Color::White);
        (void)ballTexture.loadFromImage(ballImage);
    } else { // dosyadan yüklenemezse, basit bir daire çizerek topun görünümünü oluştur
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

    // YENİ: Power-Up Texture'ı (Resim yoksa 20x20 yeşil kare oluşturur)
    sf::Texture powerUpTexture;
    if (!powerUpTexture.loadFromFile("assets/textures/powerup.png")) {
        sf::Image img({20, 20}, sf::Color::Green);
        (void)powerUpTexture.loadFromImage(img);
    }

    sf::Sprite paddle(paddleTexture);
    paddle.setScale(sf::Vector2f(
        (float)PADDLE_WIDTH / paddle.getLocalBounds().size.x, // getlocalbounds() ile texture'ın orijinal boyutunu alırız, sonra istediğimiz boyuta ölçekleriz
        (float)PADDLE_HEIGHT / paddle.getLocalBounds().size.y
    ));
    paddle.setPosition(sf::Vector2f(// ölçeklenderdinme
        (WINDOW_WIDTH - paddle.getGlobalBounds().size.x) / 2.f,
        WINDOW_HEIGHT - 50.f
    ));

    sf::Sprite ball(ballTexture);
    ball.setPosition(sf::Vector2f{WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
    sf::Vector2f ballVelocity({BALL_SPEED_X, BALL_SPEED_Y});

    std::vector<GameBrick> bricks;
    std::vector<PowerUp> powerUps; // YENİ: Düşen Power-Up'ları tutacağımız liste
    bool levelCleared = false;

     // ilk seviye tuğlalarını olsturma
int totalRows = 10;

for (int j = 0; j < totalRows; ++j) {
    int bricksInRow = 7 - (j * 2); // Satır aşağı indikçe 2 azalır: 8, 6, 4, 2
    
    // Satırı ekranda ortala
    float rowWidth = bricksInRow * (BRICK_WIDTH + BRICK_OFFSET);
    float startX = (WINDOW_WIDTH - rowWidth) / 2.f;

    for (int i = 0; i < bricksInRow; ++i) {
        GameBrick brick(brickTexture);
        brick.hp = 1;
        brick.sprite.setScale(sf::Vector2f(
            (float)BRICK_WIDTH / brick.sprite.getLocalBounds().size.x,
            (float)BRICK_HEIGHT / brick.sprite.getLocalBounds().size.y
        ));
        brick.sprite.setPosition(sf::Vector2f{
            startX + i * (BRICK_WIDTH + BRICK_OFFSET),  // ← startX'ten başla
            (float)j * (BRICK_HEIGHT + BRICK_OFFSET) + 50.f
        });
        bricks.push_back(brick);
    }
}

    while (window.isOpen()) { // oyun döngüsü, oyun açık olduğu sürece devam eder
        while (const auto event = window.pollEvent()) { // pollEvent() ile kullanıcı girişlerini ve diğer olayları kontrol ederiz
            if (event->is<sf::Event::Closed>()) window.close();// pencere kapatma olayını yakalar ve pencereyi kapatır
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && paddle.getPosition().x > 0)
            paddle.move({-PADDLE_SPEED, 0.f});
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && paddle.getPosition().x < WINDOW_WIDTH - paddle.getGlobalBounds().size.x)
            paddle.move({PADDLE_SPEED, 0.f});// sadece x ekseninde hareket eder, sağa sola gider

        ball.move(ballVelocity);

        if (ball.getPosition().x < 0 || ball.getPosition().x > WINDOW_WIDTH - ball.getGlobalBounds().size.x)
            ballVelocity.x = -ballVelocity.x;
        if (ball.getPosition().y < 0)
            ballVelocity.y = -ballVelocity.y;

        if (ball.getPosition().y > WINDOW_HEIGHT) {
            ball.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});// top alta düşünce yeniden ortaya gelir
            ballVelocity = {BALL_SPEED_X, -BALL_SPEED_Y};
        }

        if (ball.getGlobalBounds().findIntersection(paddle.getGlobalBounds()))// findIntersection() ile topun paddle ile çarpışıp çarpışmadığını kontrol ederiz
            ballVelocity.y = -std::abs(ballVelocity.y);// abs negatif hızı hep pozitif yapmaya yani topu yukarı gitmeye zorlar bu yüzden top paddle içinde sıkışmaz

        // Tuğla Çarpışmaları ve Power-Up Düşürme İhtimali
        for (auto it = bricks.begin(); it != bricks.end(); ) { // tuğla listesinde dolaşırız
            if (ball.getGlobalBounds().findIntersection(it->sprite.getGlobalBounds())) {// it->sprite ile tuğlanın sprite'ını alırız ve çarpışma kontrolü yaparız
                ballVelocity.y = -ballVelocity.y;
                it->hp--; // it aslında bir GameBrick nesnesine işaret eder, hp'sini azaltırız
                
                if (it->hp <= 0) {
                    // YENİ: %20 ihtimalle kırılan tuğladan Power-Up düşsün!
                    if (rand() % 100 < 20) {
                        PowerUp pu(powerUpTexture);
                        // Power-Up'ı tam kırılan tuğlanın merkezinden başlat
                        pu.sprite.setPosition(sf::Vector2f(
                        it->sprite.getPosition().x + (BRICK_WIDTH / 2.f) - 10.f, 
                        it->sprite.getPosition().y
                ));
                        powerUps.push_back(pu);
                    }

                    it = bricks.erase(it);// tuğla kırıldıysa listeden sileriz
                    score += 10;
                    scoreText.setString("SCORE: " + std::to_string(score) + "  LEVEL: " + std::to_string(level));
                } else {
                    it->sprite.setColor(sf::Color::White);
                }
                break;// aynı anda birden fazla tuğla kırılmasını önlemek için döngüyü kırarız
            } else {
                ++it;
            }
        }

        // YENİ: Power-Up Hareket ve Çarpışma Döngüsü
        for (auto it = powerUps.begin(); it != powerUps.end(); ) {
            it->sprite.move({0.f, 3.f}); // Aşağı doğru süzül

            // Eğer paddle'a değerse
            if (it->sprite.getGlobalBounds().findIntersection(paddle.getGlobalBounds())) {
                // Ödül: Paddle'ın boyutunu %20 uzat!
                paddle.setScale(sf::Vector2f(paddle.getScale().x + 0.2f, paddle.getScale().y));
                it = powerUps.erase(it); // Power-up'ı sil
            } 
            // Eğer yakalayamazsan ve ekrandan aşağı düşerse sil (bellek dostu)
            else if (it->sprite.getPosition().y > WINDOW_HEIGHT) {
                it = powerUps.erase(it);
            } else {
                ++it;// Diğer durumlarda sıradaki power-up'a geçilir
            }
        }
        // LEVEL 2
        if (bricks.empty() && !levelCleared) {
            levelCleared = true;
            level++;

            ball.setPosition({WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f});
            float speed = std::min(200.f + (level * 15.f), 400.f);
            ballVelocity = {speed, -speed};

            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < (3 + level); ++j) {
                    GameBrick gameBrick(brickTexture);
                    
                    if (j < 2) { // ilk 2 satır daha sağlam tuğlalar görsel farklılık katıldı
                        gameBrick.hp = 2;
                        gameBrick.sprite.setColor(sf::Color(150, 150, 150));
                    } else {
                        gameBrick.hp = 1;
                        gameBrick.sprite.setColor(sf::Color::White);
                    }

                    gameBrick.sprite.setScale(sf::Vector2f(// tuğlaların boyutunu tanımlanan sabitlere göre ölçekleriz
                        (float)BRICK_WIDTH / gameBrick.sprite.getLocalBounds().size.x,
                        (float)BRICK_HEIGHT / gameBrick.sprite.getLocalBounds().size.y
                    ));

                    gameBrick.sprite.setPosition(sf::Vector2f(
                        (float)i * (BRICK_WIDTH + BRICK_OFFSET) + 15.f,
                        (float)j * (BRICK_HEIGHT + BRICK_OFFSET) + 60.f
                    ));

                    bricks.push_back(gameBrick);
                }
            }

            scoreText.setString(
                "SCORE: " + std::to_string(score) +
                "  LEVEL: " + std::to_string(level)
            );

            levelCleared = false;
        }

        window.clear(sf::Color::Black);
        window.draw(paddle);
        window.draw(ball);
        for (auto& brick : bricks) window.draw(brick.sprite);// tuğlaları çizeriz
        
        // YENİ: Ekranda olan Power-Up'ları çiz
        for (auto& pu : powerUps) window.draw(pu.sprite); 
        
        window.draw(scoreText);
        window.display();
    }

    return 0;
}