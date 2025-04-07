#ifndef SCENE_H
#define SCENE_H

#pragma once

#include "math/geometry.h"
#include "math/camera.h"

typedef struct SceneInfo {
    int rayPerPixel;
    int width;
    int height;
    int maxRayDepth;
    int nbSpheres;
    int nbModels;
} SceneInfo;

typedef struct Scene {
    Camera* camera;
    SceneInfo* info;
    Sphere* spheres;
    Model* models;
    Vec3 ambiantLight;
} Scene;

SceneInfo scene_info_create(int rayPerPixel, int width, int height, int maxRayDepth, int nbSpheres, int nbModels) {
    SceneInfo info;
    info.rayPerPixel = rayPerPixel;
    info.width = width;
    info.height = height;
    info.maxRayDepth = maxRayDepth;
    info.nbSpheres = nbSpheres;
    info.nbModels = nbModels;
    return info;
}

Scene scene_create(Camera* cam, SceneInfo* info) {
    Scene scene;
    scene.camera = cam;
    scene.info = info;
    scene.spheres = (Sphere*)malloc(info->nbSpheres * sizeof(Sphere));
    scene.models = (Model*)malloc(info->nbModels * sizeof(Model));
    if(scene.spheres == NULL) {
        perror("Failed to allocate spheres\n");
    }
    if(scene.models == NULL) {
        perror("Failed to allocate models\n");
    }
    scene.ambiantLight = vec3_build(0.6f, 0.6f, 0.6f);
    return scene;
}

HitInfo intersect_scene(Scene* scene, Ray ray) {
    HitInfo bestHit = hitInfo_create();
    for(int i = 0; i < scene->info->nbSpheres; i++) {
        sphere_intersect(scene->spheres[i], ray, &bestHit);
    }
    for(int i = 0; i < scene->info->nbModels; i++) {
        mesh_intersect(scene->models[i], ray, &bestHit);
    }
    return bestHit;
}

void freeScene(Scene* scene) {
    free(scene->spheres);
    for(int i = 0; i < scene->info->nbModels; i++) {
        freeMesh(&scene->models[i].mesh);
    }
    free(scene->models);
}

#endif /* SCENE_H */