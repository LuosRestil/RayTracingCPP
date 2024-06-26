#include <SFML/Graphics.hpp>
#include <iostream>
#include "Vec3.h"
#include "Sphere.h"
#include "Light.h"

struct Intersection
{
    std::optional<Sphere *> sphere;
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
    Vec3 camera_pos{0, 0, -3};
    double projection_plane_distance = 1.0;
    std::vector<Sphere> spheres = {
        Sphere{Vec3{0.0, -1.0, 3.0}, 1.0, Vec3{255.0, 0.0, 0.0}, 500, 0.2},
        Sphere{Vec3{2.0, 0.0, 4.0}, 1.0, Vec3{0.0, 0.0, 255.0}, 500, 0.3},
        Sphere{Vec3{-2.0, 0.0, 4.0}, 1.0, Vec3{0.0, 255.0, 0.0}, 10, 0.4},
        Sphere{Vec3{0.0, -5001.0, 0.0}, 5000.0, Vec3{255.0, 255.0, 0.0}, 1000, 0.5}};
    std::vector<Light> lights = {
        Light{"ambient", 0.2, Vec3{0, 0, 0}},
        Light{"point", 0.6, Vec3{2.0, 1.0, 0.0}},
        Light{"directional", 0.2, Vec3{1.0, 4.0, 4.0}}};
    int max_reflections = 3;

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
                Vec3 pixel_color = trace_ray(camera_pos, direction, 0.0, INFINITY, max_reflections);
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

    Vec3 trace_ray(const Vec3 &origin, const Vec3 &direction, double min_t, double max_t, int reflections)
    {
        Intersection intersection = get_nearest_intersection(origin, direction, min_t, max_t);
        if (!intersection.sphere)
            return bg_color;

        Sphere *sphere = intersection.sphere.value();
        Vec3 intersection_point = origin + intersection.t * direction;
        Vec3 normal = (intersection_point - sphere->center).normalize();
        double intensity = calculate_lighting(intersection_point, normal, -direction, sphere->specular);
        Vec3 local_color = ((sphere->color) * intensity).clamp(0, 255);

        if (sphere->reflectiveness <= 0 || reflections <= 0)
            return local_color;

        Vec3 reflected_color = trace_ray(intersection_point, reflect_ray(-direction, normal), 0.001, INFINITY, reflections - 1);
        return (1 - sphere->reflectiveness) * local_color + sphere->reflectiveness * reflected_color;
    }

    Intersection get_nearest_intersection(const Vec3 &origin, const Vec3 &direction, double min_t, double max_t)
    {
        std::optional<Sphere *> nearest_sphere = std::nullopt;
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

    double calculate_lighting(const Vec3 &point, const Vec3 &normal, const Vec3 &view, double specular)
    {
        double intensity = 0.0;
        for (const Light &light : lights)
        {
            if (light.type == "ambient")
            {
                intensity += light.intensity;
            }
            else
            {
                Vec3 light_dir{0, 0, 0};
                if (light.type == "point")
                {
                    light_dir = light.position - point;
                }
                else // directional
                {
                    light_dir = light.position; // actually direction
                }

                // shadow
                Intersection blocker = get_nearest_intersection(point, light_dir, 0.001, 1);
                if (blocker.sphere)
                    continue;

                // diffuse
                double lDotN = light_dir.dot(normal);
                if (lDotN > 0)
                {
                    intensity += light.intensity * (lDotN / light_dir.mag());
                }

                // specular
                Vec3 reflection = reflect_ray(light_dir, normal);
                double rDotV = reflection.dot(view);
                if (rDotV > 0)
                {
                    intensity += light.intensity * std::pow(rDotV / (reflection.mag() * view.mag()), specular);
                }
            }
        }
        return intensity;
    }

    Vec3 reflect_ray(const Vec3 &ray, const Vec3 &normal)
    {
        return 2 * normal * ray.dot(normal) - ray;
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