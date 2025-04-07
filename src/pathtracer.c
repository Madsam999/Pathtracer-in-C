#include "scene.h"
#include "math/geometry.h"
#include "utils/utils.h"
#include <stdlib.h>

Vec3 getColor(const Ray ray) {
    Vec3 unitVector = vec3_normalize(ray.direction);
    float a = 0.5f * (unitVector.y + 1.0f);
    return vec3_add(vec3_mul(vec3_build(1.0f, 1.0f, 1.0f), (1.0f - a)), vec3_mul(vec3_build(0.5f, 0.7f, 1.0f), a));
}

Vec3 getTextureColor(Vec2 uv, Material mat) {
    if(mat.texture == NULL) {
        return mat.albedo;
    }

    Texture* tex = mat.texture;
    int posX = (int)(tex->width * uv.x);
    int posY = (int)(tex->height * uv.y);

    if(posY >= tex->height) {
        posY -= 1;
    }

    if(posX >= tex->width) {
        posX -= 1;
    }

    int index = (posY * tex->width + posX);

    float rChannel = (float)tex->texture[index].r / 255.0f;
    float gChannel = (float)tex->texture[index].g / 255.0f;
    float bChannel = (float)tex->texture[index].b / 255.0f;
    
    return vec3_build(rChannel, gChannel, bChannel);
}

float linearToGamma(float color) {
    if(color > 0) {
        return sqrt(color);
    }
    return 0;
}

Vec3 shade(Scene* scene, Ray* ray, HitInfo hit) {
    return vec3_add(scene->ambiantLight, hit.material.albedo);
}

Vec3 trace(Scene* scene, Ray* ray) {
    Vec3 color = vec3_build(0.0f, 0.0f, 0.0f);
    Vec3 rayColor = vec3_build(1.0f, 1.0f, 1.0f);
    for(int bounce = 0; bounce <= scene->info->maxRayDepth; bounce++) {
        HitInfo hit = intersect_scene(scene, *ray);
        if(!hit.hasHit) {
            color = vec3_add(color, vec3_vec3_mul(getColor(*ray), rayColor));
            break;
        }
        Vec3 diffuseDir = vec3_add(hit.normal, random_unit_vector());
        Vec3 specularDir = vec3_reflect(ray->direction, hit.normal);
        Vec3 newDir = vec3_lerp(diffuseDir, specularDir, hit.material.specular);
        Vec3 newOrigin = hit.hitPosition;
        ray->direction = newDir;
        ray->origin = newOrigin;

        Vec3 emittedLight = vec3_mul(hit.material.emissionColor, hit.material.emissionStrength);
        color = vec3_add(color, vec3_vec3_mul(emittedLight, rayColor));
        Vec3 hitColor = getTextureColor(hit.uv, hit.material);
        //vec3_print(hitColor);
        rayColor = vec3_vec3_mul(rayColor, hitColor);
    }

    return color;
}

unsigned char* renderScene(Scene* scene) {
    printf("Starting path tracing\n");

    int width = scene->info->width;
    int height = scene->info->height;



    // Allocate memory for the pixel data
    unsigned char *pixelData = (unsigned char *)malloc(width * height * 3 * sizeof(unsigned char));
    if (pixelData == NULL) {
        perror("Failed to allocate memory");
        return (unsigned char*) 1;
    }

    float* matrix = (float*)malloc(4 * 4 * sizeof(float));

    computeCamToWorld(scene->camera, matrix);

    // Initialize the pixel data (example: gradient pattern)
    for (int y = 0; y < height; y++) {
        printf("Scanlines left: %d\n", height - y);
        for (int x = 0; x < width; x++) {
            Vec3 avgColor = vec3_build(0.0f, 0.0f, 0.0f);
            for(int rpp = 0; rpp < scene->info->rayPerPixel; rpp++) {
                float randomOffsetX = (1.0f - (random01() * 2.0f)) / 2.0f;
                float randomOffsetY = (1.0f - (random01() * 2.0f)) / 2.0f;

                float pX = (2 * ((x + 0.5f + randomOffsetX) / (float)(width)) - 1) * tan(scene->camera->fov / 2 * PI / 180.0f) * scene->camera->aspectRatio;
                float pY = (1 - 2 * ((y + 0.5f + randomOffsetY) / (float)height)) * tan(scene->camera->fov / 2.0f * PI / 180.0f);

                Vec3 pixelPosCamSpace = vec3_build(pX, pY, -1.0f);

                Vec4 originWorld = vec4_mat4_mult(vec4_build_from_vec3(scene->camera->position, 1.0f), matrix);
                Vec3 originWorldv3 = vec3_build(originWorld.x, originWorld.y, originWorld.z);
                Vec4 pixelPos = vec4_mat4_mult(vec4_build_from_vec3(pixelPosCamSpace, 1.0f), matrix);
                Vec3 pixelPosWorld = vec3_build(pixelPos.x, pixelPos.y, pixelPos.z);

                Vec3 direction = vec3_normalize(vec3_sub(pixelPosWorld, originWorldv3));

                Ray ray = ray_create(originWorldv3, direction);

                avgColor = vec3_add(avgColor, trace(scene, &ray));
            }
            int index = (y * width + x) * 3;

            int rChannel;
            int gChannel;
            int bChannel;

            avgColor = vec3_div(avgColor, scene->info->rayPerPixel);

            vec3_clamp(vec3_build(0.0f, 0.0f, 0.0f), vec3_build(1.0f, 1.0f, 1.0f), &avgColor);

            rChannel = (int)(255.999 * avgColor.x);
            gChannel = (int)(255.999 * avgColor.y);
            bChannel = (int)(255.999 * avgColor.z);

            pixelData[index] = (rChannel);
            pixelData[index + 1] = (gChannel);
            pixelData[index + 2] = (bChannel);
        }
    }

    free(matrix);

    printf("Path tracing finished\n");

    return pixelData;
}