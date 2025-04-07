#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "math/Vectors.h"
#include "math/camera.h"
#include "utils/writePPM.h"
#include "utils/utils.h"
#include "scene.h"
#include "pathtracer.c"
#include "texture.h"

void printInformation(Camera cam, Scene scene) {
    printf("-----------------------------------------\n");
    printf("Camera Information: \n");
    printf("Vertical FOV: %.3f\n", cam.fov);
    printf("Vertical Half FOV: %.3f\n", cam.fov/2.0f);
    printf("Aspect Ratio: %.3f\n", cam.aspectRatio);
    printf("Camera Position: ");
    vec3_print(cam.position);
    printf("Camera Target: ");
    vec3_print(cam.target);
    printf("Camera Up: ");
    vec3_print(cam.up);
    printf("-----------------------------------------\n");
    printf("Scene Information: \n");
    printf("Rays Per Pixel: %d\n", scene.info->rayPerPixel);
    printf("Max Ray Depth: %d\n", scene.info->maxRayDepth);
    printf("Quantity of Spheres: %d\n", scene.info->nbSpheres);
    printf("Image Width: %d\n", scene.info->width);
    printf("Image Height: %d\n", scene.info->height);
    printf("Scene Ambiant Light: ");
    vec3_print(scene.ambiantLight);
    printf("-----------------------------------------\n");
}

void displayTexture(Texture tex) {
    printf("Displaying texture data:\n");
    for(int i = 0; i < tex.width; i++) {
        for(int j = 0; j < tex.height; j++) {
            int index = i + j * tex.width;
            printf("Color display: %d %d %d\n", tex.texture[index].r, tex.texture[index].g, tex.texture[index].b);
        }
    }
}

void createRedPPM() {
    FILE* file = fopen("red_square.ppm", "wb"); // Open in binary mode
    if (!file) {
        fprintf(stderr, "Unable to create file\n");
        return;
    }

    // Write the header
    fprintf(file, "P6\n1 1\n255\n");

    // Write the pixel data
    unsigned char red[] = {255, 0, 0};
    fwrite(red, sizeof(unsigned char), 3, file);

    fclose(file);
}

int main(int argc, char const *argv[])
{
    printf("Hello world\n");

    srand(42);

    int width, height;

    // Ask the user for the dimensions of the image
    printf("Enter the width of the image: ");
    scanf("%d", &width);
    printf("Enter the height of the image: ");
    scanf("%d", &height);

    Camera cam = camera_create(60.0f, vec3_build(0.0f, 0.0f, 0.0f), vec3_build(0.0f, 0.0f, -1.0f), vec3_build(0.0f, 1.0f, 0.0f), 1.0f, 1000.0f, (float)(width)/(float)(height));

    SceneInfo info = scene_info_create(25, width, height, 50, 5, 0);
    Scene scene = scene_create(&cam, &info);

    Texture tex = loadTexture("cc.ppm");

    Material red = material_create(vec3_build(0.0f, 1.0f, 0.0f), vec3_build(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, &tex);
    Material green = material_green();
    Material blue = material_blue();
    Material light = material_create(vec3_build(0.0f, 0.0f, 0.0f), vec3_build(1.0f, 1.0f, 1.0f), 2.0f, 0.0f, NULL);

    const char* filename = "../assets/mesh/sphere.obj";
    Mesh mesh;
    loadObj(filename, &mesh);
    printf("Mesh Size: %d\n", mesh.faceCount);

    scene.spheres[0] = sphere_create(0.5f, vec3_build(0.0f, 0.0f, -5.0f), green);
    scene.spheres[1] = sphere_create(100.0f, vec3_build(0.0f, -100.5f, -5.0f), red);
    scene.spheres[2] = sphere_create(0.75f, vec3_build(-1.0f, 0.25f, -5.5f), material_create(vec3_build(1.0f, 1.0f, 1.0f), vec3_build(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, &tex));
    scene.spheres[3] = sphere_create(10.0f, vec3_build(7.5f, 2.5f, -25.0f), light);
    scene.spheres[4] = sphere_create(20.0f, vec3_build(-7.5f, 2.5f, 25.0f), light);
    //scene.models[0] = model_create(mesh, vec3_build(0.5f, 0.0f, -5.0f), material_create(vec3_build(0.0f, 1.0f, 0.0f), vec3_build(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, &tex));

    printInformation(cam, scene);
    
    clock_t start = clock();

    unsigned char* ppmImage = renderScene(&scene);

    clock_t end = clock();

    float timeTaken = (float)(end - start) / CLOCKS_PER_SEC;

    printf("Time Taken to render in seconds: %.3f s\n", timeTaken);
    printf("Time Taken to render in minutes: %.3f min\n", timeTaken/60);
    printf("Time Taken to render in hours: %.3f h\n", timeTaken/3600);

    writePPM("test.ppm", width, height, ppmImage);
    printf("Result Drawn to image test.ppm");

    free(ppmImage);
    freeScene(&scene);
    freeTexture(&tex);

    return 0;
}
