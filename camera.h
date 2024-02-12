#ifndef CAMERA_H
#define CAMERA_H

#define MT 1
#define WRITE 1

#include "rtweekend.h"

#include "color.h"
#include "hittable.h"
#include "material.h"

#include <iostream>

#if MT
#include <execution>
#include <thread>
#endif

class camera {
    public:
        double aspect_ratio = 1.0;
        int image_width = 100;
        int samples_per_pixel = 10;
        int max_depth = 10;
        color background;

        double vfov = 90;
        point3 lookfrom = point3(0,0,-1);
        point3 lookat = point3(0,0,0);
        vec3 vup = vec3(0,1,0);

        double defocus_angle = 0;
        double focus_dist = 10;

        void render(const hittable& world) {
            std::clog << "Starting the render\n";

            initialize();

#if MT
        std::vector<int> verticalIterator, horizontalIterator;
        verticalIterator.resize(image_height);
        horizontalIterator.resize(image_width);

        for (int i = 0; i < image_height; i++) verticalIterator[i] = i;
        for (int i = 0; i < image_width; i++) horizontalIterator[i] = i;

        const int height = image_height;
        const int width = image_width;
        // vec3 colors[height][width];
        
        std::vector<std::vector<color>> colors(height, std::vector<color> (width));

        std::for_each(std::execution::par, verticalIterator.begin(), verticalIterator.end(),
        [&](int j) {
            std::for_each(std::execution::par, horizontalIterator.begin(), horizontalIterator.end(),
            [&](int i) {
            // for (int i = 0; i < image_width; ++i) {
                color pixel_color = get_pixel(world, i, j);
                pixel_color = adjust_color(pixel_color, samples_per_pixel);
                colors[j][i] += pixel_color;
            // }
            });
        });

#if WRITE
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; ++j) {
            for (int i = 0; i < image_width; ++i) {
                output_color(std::cout, colors[j][i]);
            }
        }
#endif
#else
            std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            for (int j = 0; j < image_height; ++j) {
                std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
                for (int i = 0; i < image_width; ++i) {
                    color pixel_color = get_pixel(world, i, j);
                    write_color(std::cout, pixel_color, samples_per_pixel);
                }
            }
            
            std::clog << "\nDone.\n";
#endif
        }

    private:
        int image_height;
        int sqrt_spp;
        double recip_sqrt_spp;
        point3 center;
        point3 pixel00_loc;
        vec3 pixel_delta_u;
        vec3 pixel_delta_v;
        vec3 u, v, w;
        vec3 defocus_disk_u;
        vec3 defocus_disk_v;

        void initialize() {
            image_height = static_cast<int>(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height;

            center = lookfrom;

            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta / 2);
            auto viewport_height = 2 * h * focus_dist;
            auto viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);

            sqrt_spp = static_cast<int>(sqrt(samples_per_pixel));
            recip_sqrt_spp = 1.0 / sqrt_spp;
            
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            vec3 viewport_u = viewport_width * u;
            vec3 viewport_v = viewport_height * -v;

            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height;

            auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
            pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

            auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        color get_pixel(const hittable& world, int x, int y) {
            color pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; ++s) {
                ray r = get_ray(x, y);
                pixel_color += ray_color(r, max_depth, world);
            }

            /*for (int s_j = 0; s_j < sqrt_spp; ++s_j) {
                for (int s_i = 0; s_i < sqrt_spp; ++s_i) {
                    ray r = get_ray(x, y, s_i, s_j);
                    pixel_color += ray_color(r, max_depth, world);
                }
            }*/

            return pixel_color;
        }

        ray get_ray(int i, int j) const {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto pixel_sample = pixel_center + pixel_sample_square();

            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;
            auto ray_time = random_double();

            return ray(ray_origin, ray_direction, ray_time);
        }

        ray get_ray(int i, int j, int s_i, int s_j) const {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto pixel_sample = pixel_center + pixel_sample_square(s_i, s_j);

            auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
            auto ray_direction = pixel_sample - ray_origin;
            auto ray_time = random_double();

            return ray(ray_origin, ray_direction, ray_time);
        }

        vec3 pixel_sample_square() const {
            auto px = -0.5 + random_double();
            auto py = -0.5 + random_double();

            return (px * pixel_delta_u) + (py * pixel_delta_v);
        }

        vec3 pixel_sample_square(int s_i, int s_j) const {
            auto px = -0.5 + recip_sqrt_spp * (s_i + random_double());
            auto py = -0.5 + recip_sqrt_spp * (s_j + random_double());

            return (px * pixel_delta_u) + (py * pixel_delta_v);
        }

        point3 defocus_disk_sample() const {
            // Returns a random point in the camera defocus disk.
            auto p = random_in_unit_disk();
            return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        color ray_color(const ray& r, int depth, const hittable& world) const {
            // If we've exceeded the ray bounce limit, no more light is gathered.
            // Using a pink color to accentuate where we are running out of bounces.
            if (depth <= 0) return color(1,0,1);
            
            hit_record rec;

            if (!world.hit(r, interval(0.001, infinity), rec)) return background;

            ray scattered;
            color attenuation;
            color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

            if (!rec.mat->scatter(r, rec, attenuation, scattered))
                return color_from_emission;

            double scattering_pdf = rec.mat->scattering_pdf(r, rec, scattered);
            double pdf = scattering_pdf;
            // double pdf = 1 / (2*pi);

            // color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);
            color color_from_scatter = (attenuation * scattering_pdf * ray_color(scattered, depth-1, world)) / pdf;

            return color_from_emission + color_from_scatter;
        }
};

#endif