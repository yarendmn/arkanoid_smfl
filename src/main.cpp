#include <SFML/Graphics.hpp>

int main() {
    // SFML 3'te VideoMode artık değerleri süslü parantez {} içinde vektör olarak bekler
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Arkanoid Test Penceresi");

    // Oyun döngüsü
    while (window.isOpen()) {
        
        // SFML 3'te Event (Olay) yakalama mantığı tamamen değişti
        while (const auto event = window.pollEvent()) {
            
            // Çarpı tuşuna basılıp basılmadığı artık bu şekilde kontrol ediliyor
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::Blue); // Arka planı mavi yap
        window.display(); // Çizilenleri ekrana yansıt
    }

    return 0;
}