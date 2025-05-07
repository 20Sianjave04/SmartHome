#include <SFML/Graphics.hpp>

int main() {
    // Create a window with a given width, height, and title
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");

    // Create a square shape
    sf::RectangleShape square(sf::Vector2f(50.f, 50.f));  // 50x50 size
    square.setFillColor(sf::Color::Green);  // Set square color to green
    square.setPosition(375.f, 275.f);  // Set initial position of the square

    // Movement speed of the square
    float speed = 5.f;

    // Run the program as long as the window is open
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close the window if the close event is triggered
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Move the square with arrow keys
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            square.move(-speed, 0);  // Move left
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            square.move(speed, 0);  // Move right
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            square.move(0, -speed);  // Move up
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            square.move(0, speed);  // Move down
        }

        // Clear the window with a black color
        window.clear(sf::Color::Black);

        // Draw the square
        window.draw(square);

        // Display the contents of the window
        window.display();
    }

    return 0;
}
