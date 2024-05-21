#include <SFML/Graphics.hpp>
#include <iostream>
#include "Vec3.h"
#include "Sphere.h"
#include "Light.h"

struct Intersection
{
    std::optional<Sphere*> sphere;
    double t;
};

class RayTracer
{
public:
    // window
    int window_size = 600;
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    std::vector<sf::Uint8> pixels;
    // scene
    Vec3 bg_color{0, 0, 0};
    Vec3 camera_pos{0, 0, 0};
    double projection_plane_distance = 1.0;
    std::vector<Sphere> spheres = {
        Sphere{Vec3{0.0, -1.0, 3.0}, 1.0, Vec3{255.0, 0.0, 0.0}},
        Sphere{Vec3{2.0, 0.0, 4.0}, 1.0, Vec3{0.0, 0.0, 255.0}},
        Sphere{Vec3{-2.0, 0.0, 4.0}, 1.0, Vec3{0.0, 255.0, 0.0}},
        Sphere{Vec3{0.0, -5001.0, 0.0}, 5000.0, Vec3{255.0, 255.0, 0.0}}};
    std::vector<Light> lights = {
        Light{"ambient", 0.2, Vec3{0, 0, 0}},
        Light{"point", 0.6, Vec3{2.0, 1.0, 0.0}},
        Light{"directional", 0.2, Vec3{1.0, 4.0, 4.0}}};

    RayTracer() : window(sf::VideoMode(window_size, window_size), "Ray Tracing"),
                  pixels(window_size * (window_size * 4))
    {
        window.setVerticalSyncEnabled(true);
        texture.create(window_size, window_size);
        sprite.setTexture(texture);
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

            update_pixels();
            draw();
        }
    }

    void update_pixels()
    {
        for (int y = window_size / 2; y > -window_size / 2; y--)
        {
            for (int x = -window_size / 2; x < window_size / 2; x++)
            {
                Vec3 direction = camera_to_viewport(x, y);
                Vec3 pixel_color = trace_ray(camera_pos, direction, 0.0, INFINITY);
                put_pixel(x, y, pixel_color);
            }
        }
    }

    Vec3 camera_to_viewport(int x, int y)
    {
        return {
            static_cast<double>(x) / window_size,
            static_cast<double>(y) / window_size,
            projection_plane_distance};
    }

    Vec3 trace_ray(const Vec3 &origin, const Vec3 &direction, double min_t, double max_t)
    {
        Intersection intersection = get_nearest_intersection(origin, direction, min_t, max_t);
        if (!intersection.sphere)
            return bg_color;
        
        Sphere *sphere = intersection.sphere.value();
        return sphere->color;
    }

    Intersection get_nearest_intersection(const Vec3 &origin, const Vec3 &direction, double min_t, double max_t)
    {
        std::optional<Sphere*> nearest_sphere = std::nullopt;
        double nearest_t = INFINITY;

        for (Sphere &sphere : spheres)
        {
            std::array<double, 2> ts = intersect_ray_sphere(origin, direction, sphere);
            double t1 = ts[0];
            double t2 = ts[1];
            if (t1 < nearest_t && t1 > min_t && t1 < max_t)
            {
                nearest_t = t1;
                nearest_sphere = &sphere;
            }
            if (t2 < nearest_t && t2 > min_t && t2 < max_t)
            {
                nearest_t = t2;
                nearest_sphere = &sphere;
            }
        }
        return {nearest_sphere, nearest_t};
    }

    std::array<double, 2> intersect_ray_sphere(const Vec3 &origin, const Vec3 &direction, const Sphere &sphere)
    {
        Vec3 CO = origin - sphere.center;
        double a = direction.dot(direction);
        double b = 2 * direction.dot(CO);
        double c = CO.dot(CO) - sphere.radius * sphere.radius;
        double discriminant = b * b - 4 * a * c;

        if (discriminant < 0)
            return {INFINITY, INFINITY};

        double t1 = (-b + std::sqrt(discriminant)) / (2 * a);
        double t2 = (-b - std::sqrt(discriminant)) / (2 * a);
        return {t1, t2};
    }

    void put_pixel(int x, int y, const Vec3 &pixel_color)
    {
        x = window_size / 2 + x;
        y = window_size / 2 - y;
        int offset = y * window_size * 4 + x * 4;
        pixels[offset++] = pixel_color.x;
        pixels[offset++] = pixel_color.y;
        pixels[offset++] = pixel_color.z;
        pixels[offset] = 255;
    }

    void draw()
    {
        texture.update(pixels.data());
        window.clear();
        window.draw(sprite);
        window.display();
    }
};

int main()
{
    RayTracer().run();
}