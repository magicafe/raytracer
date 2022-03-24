#include "headers.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <semaphore>
#include "color.h"
#include "geometry/sphere.h"
#include "geometry/hittable_list.h"
#include "camera.h"
#include "material/lambertian.h"
#include "material/metal.h"
#include "material/dielectric.h"
#include "scene.h"
#include "cmd/cmd_opts.h"

using namespace std::chrono_literals;

// Image
double aspect_ratio = 3.0 / 2.0;
const int SAMPLES_PER_PIXEL = 500;
const int MAX_DEPTH = 50;
int image_width = 1200;
int image_height = static_cast<int>(image_width / aspect_ratio);

std::atomic<int> progress{0};
std::mutex m1;
std::atomic<int> runningThreadCount{0};

color ray_color(const ray &r, const color &background, const hittable &world, int depth)
{
    hit_record rec;

    if (depth <= 0)
    {
        return color::zero();
    }

    if (!world.hit(r, 0.001, infinity, rec))
    {
        return background;
    }
    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    {
        return emitted;
    }
    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

// [start, end]
void scan_vertical(std::vector<rgb> &colors, int start, int end, const color &background, const camera &camera, const hittable &world)
{
    m1.lock();
    std::cerr << "Scan from " << start << " to " << end << "\n";
    runningThreadCount++;
    m1.unlock();
    for (int j = start; j >= end; --j)
    {
        // std::cerr << "Scanlines remaining: " << j - end << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < SAMPLES_PER_PIXEL; ++s)
            {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = camera.get_ray(u, v);
                pixel_color += ray_color(r, background, world, MAX_DEPTH);
            }

            write_color(colors, pixel_color, SAMPLES_PER_PIXEL);
        }
        progress--;
    }
}

#include <Eigen/Dense>

int main(int argc, const char *argv[])
{
    struct options
    {
        string scene_name{"scene"};
        int image_width{1920};
        string image_output{"image"};
    };

    auto parser = cmd_opts<options>::create(
        {{"-scene", &options::scene_name},
         {"-width", &options::image_width},
         {"-image", &options::image_output}});

    auto configs = parser->parse(argc, argv);
    image_width = configs.image_width;
    image_height = static_cast<int>(image_width / aspect_ratio);

    // World
    point3 lookfrom;
    point3 lookat;
    auto vfov = 40.0;
    auto aperture = 0.0;
    color background(0, 0, 0);

    bvh_node world;
    if (configs.scene_name.compare("spheres") == 0)
    {
        world = two_spheres();
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
    }
    else if (configs.scene_name.compare("perlin_spheres") == 0)
    {
        world = two_perlin_spheres();
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
    }
    else if (configs.scene_name.compare("earth") == 0)
    {
        world = earth();
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
    }
    else if (configs.scene_name.compare("simple_light") == 0)
    {
        world = simple_light();
        background = color(0, 0, 0);
        lookfrom = point3(26, 3, 6);
        lookat = point3(0, 2, 0);
        vfov = 20.0;
    }
    else if (configs.scene_name.compare("cornell") == 0)
    {
        world = cornell_box();
        aspect_ratio = 1.0;
        image_width = 600;
        image_height = static_cast<int>(image_width / aspect_ratio);
        background = color(0, 0, 0);
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
    }
    else
    {
        world = random_scene();
        background = color(0.70, 0.80, 1.00);
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        aperture = 0.1;
    }

    // Camera
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10;
    camera camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0, 1);

    auto start_time = std::chrono::high_resolution_clock::now();

    int thread_count = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    std::vector<std::vector<rgb>> final_colors;
    for (int i = 0; i < thread_count; ++i)
    {
        final_colors.push_back(std::vector<rgb>());
    }

    progress = image_height;

    int vertical_step = image_height / thread_count;
    int start = image_height - 1;
    int end = start - vertical_step;

    for (int i = 0; i < thread_count; ++i)
    {
        start = image_height - 1 - vertical_step * i;
        end = start - vertical_step;
        if (i == thread_count - 1)
        {
            end = 0;
        }
        threads.push_back(std::thread(scan_vertical, std::ref(final_colors[i]), start, end, std::ref(background), std::ref(camera), std::ref(world)));
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
              << image_width << " " << image_height << "\n255\n";
    for (auto &colors : final_colors)
    {
        for (auto &color : colors)
        {
            std::cout << color.r << ' ' << color.g << ' ' << color.b << '\n';
        }
    }
    std::cerr << "\nDone.\n";

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;
    std::cerr << "Pantry time: " << diff.count() << "s" << std::endl;
}
