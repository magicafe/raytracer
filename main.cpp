#include "headers.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <semaphore>
#include "color.h"
#include "sphere.h"
#include "hittable_list.h"
#include "camera.h"

using namespace std::chrono_literals;

// Image
const double ASPECT_RATIO = 16.0 / 9.0;
const int IMAGE_WIDTH = 1920;
const int IMAGE_HEIGHT = static_cast<int>(IMAGE_WIDTH / ASPECT_RATIO);
const int SAMPLES_PER_PIXEL = 100;
const int MAX_DEPTH = 50;

std::atomic<int> progress{0};
std::mutex m1;
std::atomic<int> runningThreadCount{0};

color ray_color(const ray &r, const hittable &world, int depth)
{
    hit_record rec;

    if (depth <= 0)
    {
        return color::zero();
    }

    if (world.hit(r, 0, infinity, rec))
    {
        point3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color::identity() + t * color(0.5, 0.7, 1.0);
}

// [start, end]
void scan_vertical(std::vector<rgb> &colors, int start, int end, const camera &camera, const hittable &world)
{
    m1.lock();
    std::cerr << "Scan from " << start << " to " << end << "\n";
    runningThreadCount++;
    m1.unlock();
    for (int j = start; j >= end; --j)
    {
        // std::cerr << "Scanlines remaining: " << j - end << ' ' << std::flush;
        for (int i = 0; i < IMAGE_WIDTH; ++i)
        {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < SAMPLES_PER_PIXEL; ++s)
            {
                auto u = (i + random_double()) / (IMAGE_WIDTH - 1);
                auto v = (j + random_double()) / (IMAGE_HEIGHT - 1);
                ray r = camera.get_ray(u, v);
                pixel_color += ray_color(r, world, MAX_DEPTH);
            }

            write_color(colors, pixel_color, SAMPLES_PER_PIXEL);
        }
        progress--;
    }
}

int main(int, char **)
{
    // World
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera
    camera camera;

    int thread_count = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    std::vector<std::vector<rgb>> final_colors;
    for (int i = 0; i < thread_count; ++i)
    {
        final_colors.push_back(std::vector<rgb>());
    }

    progress = IMAGE_HEIGHT;

    int vertical_step = IMAGE_HEIGHT / thread_count;
    int start = IMAGE_HEIGHT - 1;
    int end = start - vertical_step;

    for (int i = 0; i < thread_count; ++i)
    {
        threads.push_back(std::thread(scan_vertical, std::ref(final_colors[i]), start, end, std::ref(camera), std::ref(world)));
        start = end - 1;
        end = start - vertical_step;
        if (end < 0)
        {
            end = 0;
        }
    }

    while (runningThreadCount != thread_count)
    {
        std::this_thread::sleep_for(1000ms);
    }

    while (progress >= 0)
    {
        std::cerr << "\rScanlines remainning: " << progress << ' ' << std::flush;
        std::this_thread::sleep_for(2000ms);
        if (progress == 0)
        {
            break;
        }
    }

    for (auto &th : threads)
    {
        th.join();
    }

    // Render
    std::cout << "P3\n"
              << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";
    for (auto &colors : final_colors)
    {
        for (auto &color : colors)
        {
            std::cout << color.r << ' ' << color.g << ' ' << color.b << '\n';
        }
    }
    std::cerr << "\nDone.\n";
}
