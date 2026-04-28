#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

int main() {
    // Pencereyi oluştur ve FPS'i 60'a sabitle (oyun motoru çok hızlı çalışıp topu ışınlamasın diye)
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Arkanoid - Kutsal Uclu");
    window.setFramerateLimit(60);

    // 1. PADDLE (ÇUBUK) OLUŞTURMA
    // Genişlik: 100, Yükseklik: 20
    sf::RectangleShape paddle(sf::Vector2f({100.f, 20.f}));
    paddle.setFillColor(sf::Color::White);
    paddle.setPosition({350.f, 550.f}); // Ekranın alt-ortasına yerleştir

    // 2. TOP OLUŞTURMA
    sf::CircleShape ball(10.f); // 10 piksel yarıçap
    ball.setFillColor(sf::Color(255,182,193)); // Pembe renk (klasik retro tarzı)
    ball.setPosition({400.f, 300.f}); // Ekranın tam ortasına yerleştir

    // Topun hareket hızı (X ekseninde 5, Y ekseninde -5 hızında başlasın)
    sf::Vector2f ballVelocity({5.f, -5.f}); 

    // Oyun Döngüsü
    while (window.isOpen()) {
        
        // Çarpı (X) tuşuna basılma kontrolü
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // --- GÜNCELLEME (UPDATE) KISMI ---

        // PADDLE HAREKETİ (Sağ ve Sol ok tuşları)
        // Sınırları aşmaması için x > 0 ve x < 700 (800 ekran genişliği - 100 paddle genişliği) kontrolü yapıyoruz
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && paddle.getPosition().x > 0.f) {
            paddle.move({-8.f, 0.f});
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && paddle.getPosition().x < 700.f) {
            paddle.move({8.f, 0.f});
        }

        // TOP HAREKETİ VE DUVARLARDAN SEKME
        ball.move(ballVelocity);

        // Sol veya Sağ duvara çarpma kontrolü
        if (ball.getPosition().x <= 0.f || ball.getPosition().x >= 800.f - 20.f) {
            ballVelocity.x = -ballVelocity.x; // X yönünü tersine çevir (sekme efekti)
        }
        // Üst veya Alt duvara çarpma kontrolü 
        // (Şimdilik test için alt duvardan da seksin, normalde aşağı düşünce canımız gidecek)
        if (ball.getPosition().y <= 0.f || ball.getPosition().y >= 600.f - 20.f) {
            ballVelocity.y = -ballVelocity.y; // Y yönünü tersine çevir
        }

        // TOP - PADDLE ÇARPIŞMASI
        if (ball.getGlobalBounds().findIntersection(paddle.getGlobalBounds())) {
            ballVelocity.y = -ballVelocity.y;

    // Top paddle'ın neresine çarptıysa ona göre yön değişsin (daha gerçekçi)
        float paddleCenter = paddle.getPosition().x + 50.f;
        float ballCenter = ball.getPosition().x + 10.f;
        float fark = ballCenter - paddleCenter;

        ballVelocity.x = fark * 0.1f; // açı veriyoruz
        }
        // --- ÇİZİM (RENDER) KISMI ---
        window.clear(sf::Color::Black); // Arka plan siyah olsun (klasik retro tarzı)
        
        window.draw(paddle); // Çubuğu çiz
        window.draw(ball);   // Topu çiz
        
        window.display();    // Ekrana yansıt
    }

    return 0;
}