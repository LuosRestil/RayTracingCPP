#include <SFML/Graphics.hpp>
#include <iostream>

class RayTracer
{
public:
    int window_size = 600;
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    std::vector<sf::Uint8> pixels;

    RayTracer() : 
        window(sf::VideoMode(window_size, window_size), "Ray Tracing"),
        sprite(texture),
        pixels(window_size * (window_size * 4)) 
    {
        window.setVerticalSyncEnabled(true);
    }

    void run()
    {
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }

            updatePixels();
            window.draw(sprite);
            window.display();
        }
    }

    void updatePixels() {}
};

int main()
{
    RayTracer().run();
}