#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "color.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"

#include <chrono>

void random_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    // auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    // world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - point3(4,0.2,0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if(choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0, .5), 0);
                
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    world = hittable_list(make_shared<bvh_node>(world));

    camera cam;
    
    cam.aspect_ratio        = 16.0 / 9.0;
    cam.image_width         = 400;
    cam.samples_per_pixel   = 100;
    cam.max_depth           = 50;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov            = 20;
    cam.lookfrom        = point3(13,2,3);
    cam.lookat          = point3(0,0,0);
    cam.vup             = vec3(0,1,0);

    cam.defocus_angle   = 0.02;
    cam.focus_dist      = 10.0;

    cam.render(world);
}

void two_spheres() {
    hittable_list world;

    auto checker = make_shared<checker_texture>( .8, color(.2, .3, .1), color(.9, .9, .9));

    world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    camera cam;

    cam.aspect_ratio        = 16.0 / 9.0;
    cam.image_width         = 400;
    cam.samples_per_pixel   = 100;
    cam.max_depth           = 50;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov            = 20;
    cam.lookfrom        = point3(13,2,3);
    cam.lookat          = point3(0,0,0);
    cam.vup             = vec3(0,1,0);

    cam.defocus_angle   = 0;

    cam.render(world);
}

void earth() {
    auto earth_texture = make_shared<image_texture>("textures/earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = point3(0,0,12);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(hittable_list(globe));
}

void two_perlin_spheres() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);

    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void quads() {
    hittable_list world;

    // Materials
    auto left_red       = make_shared<lambertian>(color(1.0, .2, .2));
    auto back_green     = make_shared<lambertian>(color(.2, 1.0, .2));
    auto right_blue     = make_shared<lambertian>(color(.2, .2, 1.0));
    auto upper_orange   = make_shared<lambertian>(color(1.0, .5, .0));
    auto lower_teal     = make_shared<lambertian>(color(.2, .8, .8));

    // Quads
    world.add(make_shared<quad>(point3(-3, -2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2, -2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3( 3, -2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2,  3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2, -3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0.70, 0.80, 1.00);

    cam.vfov     = 80;
    cam.lookfrom = point3(0,0,9);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void simple_light() {
    hittable_list world;

    auto pertext = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
    world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    world.add(make_shared<sphere>(point3(0,7,0), 2, difflight));
    world.add(make_shared<quad>(point3(3,1,-2), vec3(2,0,0), vec3(0,2,0), difflight));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0, 0, 0);

    cam.vfov     = 20;
    cam.lookfrom = point3(26,3,6);
    cam.lookat   = point3(0,2,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void cornell_box() {
    hittable_list world;

    auto red    = make_shared<lambertian>(color(.65, .05, .05));
    auto white  = make_shared<lambertian>(color(.73));
    auto green  = make_shared<lambertian>(color(.12, .45, .15));
    auto light  = make_shared<diffuse_light>(color(15));

    /* world.add(make_shared<quad>(point3(555,0,0),    vec3(0,555,0),  vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0),      vec3(0,555,0),  vec3(0,0,555), red));
    world.add(make_shared<quad>(point3(0,0,0),      vec3(555,0,0),  vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(555,555,555),vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(0,0,555),    vec3(555,0,0),  vec3(0,555,0), white)); */
    world.add(make_shared<quad>(point3(555,0,0), vec3(0,0,555), vec3(0,555,0), green));
    world.add(make_shared<quad>(point3(0,0,555), vec3(0,0,-555), vec3(0,555,0), red));
    world.add(make_shared<quad>(point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(555,0,555), vec3(-555,0,0), vec3(0,555,0), white));
    
    // world.add(make_shared<quad>(point3(343,554,332), vec3(-130,0,0),vec3(0,0,-105), light));
    world.add(make_shared<quad>(point3(213,554,227), vec3(130,0,0), vec3(0,0,105), light));

    // world.add(box(point3(130,0,65), point3(295,165,230), white));
    // world.add(box(point3(265,0,295), point3(430,330,460), white));

    shared_ptr<hittable> box1 = box(point3(0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    world.add(box1);

    shared_ptr<hittable> box2 = box(point3(0), point3(165), white);
    box2 = make_shared<rotate_y>(box2,-18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    world.add(box2);

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    // cam.samples_per_pixel = 64;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = color(0);

    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void cornell_smoke() {
    hittable_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(point3(113,554,127), vec3(330,0,0), vec3(0,0,305), light));
    world.add(make_shared<quad>(point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    shared_ptr<hittable> box1 = box(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));

    shared_ptr<hittable> box2 = box(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));

    world.add(make_shared<constant_medium>(box1, 0.01, color(0,0,0)));
    world.add(make_shared<constant_medium>(box2, 0.01, color(1,1,1)));

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    // cam.samples_per_pixel = 200;
    cam.samples_per_pixel = 50;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void final_scene(int image_width, int samples_per_pixel, int max_depth) {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    // Floor boxes
    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i *w;
            auto z0 = -1000.0 + j *w;
            auto y0 = 0.0;

            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 +w;

            boxes1.add(box(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(color(7,7,7));
    world.add(make_shared<quad>(point3(123,554,147), vec3(300,0,0), vec3(0,0,265), light));

    // Moving sphere
    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    // Glass ball
    world.add(make_shared<sphere>(point3(260,150,45), 50, make_shared<dielectric>(1.5)));

    // Metal ball
    world.add(make_shared<sphere>(point3(0,150,145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));

    // Blue SSS ball
    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));

    // Scene fog
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    // Earth ball
    auto emat = make_shared<lambertian>(make_shared<image_texture>("textures/earthmap.jpg"));
    world.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));

    // Perlin ball
    auto pertext = make_shared<noise_texture>(0.1);
    world.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));

    // Cube of balls
    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73,.73,.73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    world.add(
        make_shared<translate>(
            make_shared<rotate_y>(
                make_shared<bvh_node>(boxes2), 15),
                vec3(-100, 270, 395)
        )
    );

    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0, 0, 0);

    cam.vfov     = 40;
    cam.lookfrom = point3(478,278,-600);
    cam.lookat   = point3(278,278,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void density_test() {
    hittable_list world;

    // auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    // world.add(make_shared<sphere>(point3(278, -1000, 0), 1000, make_shared<lambertian>(checker)));
    
    auto ground = make_shared<lambertian>(color(.27));
    world.add(make_shared<quad>(point3(-5000, 70, -5000), vec3(10000, 0, 0), vec3(0, 0,10000), ground));

    auto light = make_shared<diffuse_light>(color(10));
    world.add(make_shared<quad>(point3(50,554,147), vec3(300,0,0), vec3(0,0,265), light));
    
    world.add(make_shared<quad>(point3(-25,125,250), vec3(450,0,0), vec3(0,20,0), light));

    // Blue SSS ball
    auto boundary = make_shared<sphere>(point3(410,130,145), 50, make_shared<dielectric>(1.5));
    world.add(boundary);
    // world.add(make_shared<constant_medium>(boundary, 0.0001, color(0.2, 0.4, 0.9)));

    boundary = make_shared<sphere>(point3(300,130,145), 50, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.0001, color(0.2, 0.4, 0.9)));

    boundary = make_shared<sphere>(point3(190,130,145), 50, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.001, color(0.2, 0.4, 0.9)));

    boundary = make_shared<sphere>(point3(80,130,145), 50, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.01, color(0.2, 0.4, 0.9)));

    boundary = make_shared<sphere>(point3(-30,130,145), 50, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.1, color(0.2, 0.4, 0.9)));

    camera cam;

    cam.aspect_ratio      = 1.0;
    // cam.image_width       = 800;
    cam.image_width       = 600;
    // cam.image_width       = 400;
    // cam.samples_per_pixel = 250;
    cam.samples_per_pixel = 500;
    cam.max_depth         = 16;
    cam.background        = color(0, 0, 0);

    cam.vfov     = 45;
    cam.lookfrom = point3(190,278,-600);
    cam.lookat   = point3(190,278,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void bubble() {
    hittable_list world;

    // auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));
    // world.add(make_shared<sphere>(point3(278, -1000, 0), 1000, make_shared<lambertian>(checker)));
    
    auto ground = make_shared<lambertian>(color(.27));
    world.add(make_shared<quad>(point3(-5000, 70, -5000), vec3(10000, 0, 0), vec3(0, 0,10000), ground));

    auto light = make_shared<diffuse_light>(color(10));
    world.add(make_shared<quad>(point3(50,554,147), vec3(300,0,0), vec3(0,0,265), light));

    auto outer = make_shared<sphere>(point3(190,160,145), 80, make_shared<dielectric>(1.5));
    world.add(outer);
    auto inner = make_shared<sphere>(point3(190,160,145), -75, make_shared<dielectric>(1.5));
    world.add(inner);

    outer = make_shared<sphere>(point3(370,160,145), 80, make_shared<dielectric>(1.5));
    world.add(outer);
    inner = make_shared<sphere>(point3(370,160,145), -60, make_shared<dielectric>(1.5));
    world.add(inner);

    outer = make_shared<sphere>(point3(10,160,145), 80, make_shared<dielectric>(1.5));
    world.add(outer);
    inner = make_shared<sphere>(point3(10,160,145), -40, make_shared<dielectric>(1.5));
    world.add(inner);

    // auto emat = make_shared<diffuse_light>(make_shared<image_texture>("textures/earthmap.jpg"));
    // world.add(make_shared<sphere>(point3(190,160,145), -1500, emat));

    camera cam;

    cam.aspect_ratio      = 1.0;
    // cam.image_width       = 800;
    cam.image_width       = 600;
    // cam.image_width       = 400;
    // cam.samples_per_pixel = 250;
    // cam.samples_per_pixel = 500;
    cam.samples_per_pixel = 50;
    cam.max_depth         = 16;
    cam.background        = color(0, 0, 0);

    cam.vfov     = 45;
    cam.lookfrom = point3(190,278,-600);
    cam.lookat   = point3(190,278,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main() {
    auto start = std::chrono::system_clock::now();

    switch (7) {
        case 1: random_spheres();                 break;
        case 2: two_spheres();                  break;
        case 3: earth();                        break;
        case 4: two_perlin_spheres();           break;
        case 5: quads();                        break;
        case 6: simple_light();                 break;
        case 7: cornell_box();                  break;
        case 8: cornell_smoke();                break;
        case 9: final_scene(800, 10000, 40);    break;
        case 10: density_test();                break;
        case 11: bubble();                      break;
        // default: final_scene(400, 250, 16);      break;
        default: final_scene(800, 1000, 16);    break;
    }

    auto end = std::chrono::system_clock::now();
    std::clog << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
}