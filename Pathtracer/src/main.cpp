#include <Color.hpp>
#include <Ray.hpp>
#include <Vector.hpp>
#include <Sphere.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <random>

// screen
constexpr const unsigned int IMAGE_WIDTH = 1920;
constexpr const unsigned int IMAGE_HEIGHT = 1080;
constexpr const double ASPECT_RATIO = static_cast<double>(IMAGE_WIDTH) / IMAGE_HEIGHT;
constexpr const double RAY_MAX_LEN = 10.0;
constexpr const double RAY_MIN_LEN = 0.0;

// objects
std::vector<std::unique_ptr<Hittable>> objects;

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

dVec3 ray_color(const Ray& r) {
    HitRecord rec;
    HitRecord temp_rec;
    bool hit_anything = false;
    auto closest_so_far = RAY_MAX_LEN;

    for (const auto& object : objects) {
        if (object->hit(r, RAY_MIN_LEN, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    if (hit_anything) {
        return 0.5 * (rec.normal + dVec3{ 1, 1, 1 });
    }
    else {
        dVec3 unit_direction = r.direction().normalized();
        auto a = 0.5 * (unit_direction[1] + 1.0);
        return (1.0 - a) * dVec3 { 1.0, 1.0, 1.0 } + a * dVec3{ 0.5, 0.7, 1.0 };
    }
}

int main() {
    // Camera
    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width = viewport_height * ASPECT_RATIO;
    auto camera_center = dVec3{ 0.0, 0.0, 0.0 };

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = dVec3{ viewport_width, 0.0, 0.0 };
    auto viewport_v = dVec3{ 0.0, -viewport_height, 0.0 };

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    auto pixel_delta_u = viewport_u / IMAGE_WIDTH;
    auto pixel_delta_v = viewport_v / IMAGE_HEIGHT;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left = camera_center - dVec3{ 0.0, 0.0, focal_length } - viewport_u / 2.0 - viewport_v / 2.0;
    auto pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    objects.push_back(std::make_unique<Sphere>(dVec3{ 0.0, 0.0, -1.0 }, 0.5));
    objects.push_back(std::make_unique<Sphere>(dVec3{ 0.0, -100.5, -1.0 }, 100));

    // Render
    std::cout << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";
    for (int j = 0; j < IMAGE_HEIGHT; ++j) {
        std::clog << "\rScanlines remaining: " << (IMAGE_HEIGHT - j) << ' ' << std::flush;
        for (int i = 0; i < IMAGE_WIDTH; ++i) {
            auto pixel_center = pixel00_loc + (static_cast<double>(i) * pixel_delta_u) + (static_cast<double>(j) * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            Ray r(camera_center, ray_direction);

            dVec3 pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                 \n";
}