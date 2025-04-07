#ifndef GEOMETRY_H
#define GEOMETRY_H

#pragma once

#include <float.h>

#include "Vectors.h"
#include "../texture.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#define EPSILON 1e-6

typedef struct {
    Vec3 albedo;
    Vec3 emissionColor;
    float emissionStrength;
    float specular;
    Texture* texture;
} Material;

typedef struct HitInfo {
    int hasHit;
    float hitDistance;
    Vec3 hitPosition;
    Material material;
    Vec3 normal;
    Vec2 uv;
} HitInfo;

typedef struct Ray {
    Vec3 origin;
    Vec3 direction;
} Ray;

typedef struct Sphere {
    float radius;
    Vec3 center;
    Material material;
} Sphere;

typedef struct Face {
    int v[3];
    int vt[3];
    int vn[3];
} Face;

typedef struct Mesh {
    Vec3* vertices;
    Vec3* normals;
    Vec2* uvs;
    Face* faces;

    int vertexCount, normalCount, uvCount, faceCount;
} Mesh;

typedef struct Model {
    Mesh mesh;
    Vec3 center;
    Material material;
} Model;

Material material_create(Vec3 albedo, Vec3 emissionColor, float emissionStrength, float specular, Texture* texture) {
    Material mat;
    mat.albedo = albedo;
    mat.emissionColor = emissionColor;
    mat.emissionStrength = emissionStrength;
    mat.specular = specular;
    mat.texture = texture;
    return mat;
}

Material material_white() {
    Material white;
    white.albedo = vec3_build(1.0f, 1.0f, 1.0f);
    white.emissionColor = vec3_build(0.0f, 0.0f, 0.0f);
    white.emissionStrength = 0.0f;
    white.specular = 0.5f;
    white.texture = NULL;
    return white;
}

Material material_red() {
    Material red;
    red.albedo = vec3_build(1.0f, 0.0f, 0.0f);
    red.emissionColor = vec3_build(1.0f, 0.9f, 0.7f);
    red.emissionStrength = 0.0f;
    red.texture = NULL;
    return red;
}

Material material_green() {
    Material green;
    green.albedo = vec3_build(0.0f, 1.0f, 0.0f);
    green.emissionColor = vec3_build(0.0f, 0.0f, 0.0f);
    green.emissionStrength = 0.0f;
    green.specular = 0.5f;
    green.texture = NULL;
    return green;
}

Material material_blue() {
    Material blue;
    blue.albedo = vec3_build(0.0f, 0.0f, 1.0f);
    blue.emissionColor = vec3_build(0.0f, 0.0f, 0.0f);
    blue.emissionStrength = 0.0f;
    blue.specular = 1.0f;
    blue.texture = NULL;
    return blue;
}

Sphere sphere_create(float radius, Vec3 center, Material material) {
    Sphere sphere;
    sphere.radius = radius;
    sphere.center = center;
    sphere.material = material;
    return sphere;
}

Model model_create(Mesh mesh, Vec3 center, Material mat) {
    Model model;
    model.mesh = mesh;
    for(int i = 0; i < mesh.vertexCount; i++) {
        mesh.vertices[i] = vec3_add(mesh.vertices[i], center);
    }
    model.center = center;
    model.material = mat;
    return model;
}

int loadObj(const char *filename, Mesh *mesh) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Unable to open file %s\n", filename);
        return 0;
    }

    mesh->vertices = NULL;
    mesh->vertexCount = 0;
    mesh->normals = NULL;
    mesh->normalCount = 0;
    mesh->uvs = NULL;
    mesh->uvCount = 0;
    mesh->faces = NULL;
    mesh->faceCount = 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "v ", 2) == 0) {
            Vec3 v;
            if (sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z) == 3) {
                mesh->vertices = realloc(mesh->vertices, (mesh->vertexCount + 1) * sizeof(Vec3));
                mesh->vertices[mesh->vertexCount] = v;
                mesh->vertexCount++;
            }
        } else if (strncmp(line, "vt ", 3) == 0) {
            Vec2 uv;
            if (sscanf(line, "vt %f %f", &uv.x, &uv.y) == 2) {
                mesh->uvs = realloc(mesh->uvs, (mesh->uvCount + 1) * sizeof(Vec2));
                mesh->uvs[mesh->uvCount] = uv;
                mesh->uvCount++;
            }
        } else if (strncmp(line, "vn ", 3) == 0) {
            Vec3 n;
            if (sscanf(line, "vn %f %f %f", &n.x, &n.y, &n.z) == 3) {
                mesh->normals = realloc(mesh->normals, (mesh->normalCount + 1) * sizeof(Vec3));
                mesh->normals[mesh->normalCount] = n;
                mesh->normalCount++;
            }
        } else if (strncmp(line, "f ", 2) == 0) {
            Face f = {0};
            int vert[3] = {0}, tex[3] = {0}, norm[3] = {0};
            for (int i = 0; i < 3; i++) {
                vert[i] = tex[i] = norm[i] = -1;
            }
            
            int matches = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vert[0], &tex[0], &norm[0],
                &vert[1], &tex[1], &norm[1],
                &vert[2], &tex[2], &norm[2]);
            
            if (matches != 9) {
                fprintf(stderr, "bad line: %s\n", line);
                continue;
            }

            for(int i = 0; i < 3; i++) {
                f.v[i] = vert[i] - 1 ;
                f.vt[i] = tex[i] - 1;
                f.vn[i] = norm[i] - 1;
            }
            
            // Save
            mesh->faces = realloc(mesh->faces, (mesh->faceCount + 1) * sizeof(Face));
            mesh->faces[mesh->faceCount++] = f;
        }
    }

    fclose(file);
    return 1;
}

Ray ray_create(Vec3 origin, Vec3 direction) {
    Ray ray;
    ray.origin = origin;
    ray.direction = direction;
    return ray;
}

HitInfo hitInfo_create() {
    HitInfo info;
    info.hasHit = 0;
    info.hitDistance = FLT_MAX;
    info.material = material_create(vec3_build(0.0f, 0.0f, 0.0f), vec3_build(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, NULL);
    return info;
}

Vec3 ray_hit_position(Ray ray, float t) {
    return vec3_add(ray.origin, vec3_mul(ray.direction, t));
}

void sphere_intersect(Sphere sphere, Ray ray, HitInfo* info) {
    //printf("Sphere Intersect test\n");
    Vec3 oc = vec3_sub(sphere.center, ray.origin);
    float a = vec3_dot(ray.direction, ray.direction);
    float b = -2.0f * vec3_dot(ray.direction, oc);
    float c = vec3_dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;

    if(discriminant >= 0) {
        float tMin = (-b - sqrt(discriminant)) / (2.0f * a);
        if(tMin > 0.0f && tMin < info->hitDistance) {
            info->hitDistance = tMin;
            info->material = sphere.material;
            info->hasHit = 1;
            Vec3 hitPosition = ray_hit_position(ray, tMin);
            info->hitPosition = hitPosition;
            info->normal = vec3_normalize(vec3_sub(hitPosition, sphere.center));

            float theta = acos(hitPosition.y / sphere.radius);
            float phi = atan2(hitPosition.x, hitPosition.z);
            if(phi < 0.0f) {
                phi += 2 * PI;
            }

            float u = phi / (2.0f * PI);
            float v = theta / PI;
            info->uv = vec2_build(u, v);
        }
    }
}

void face_intersect(Vec3* verts, Vec3* norms, Vec2* uvs, Ray ray, HitInfo* info, Material mat) {
    Vec3 p1 = verts[0];
    Vec3 p2 = verts[1];
    Vec3 p3 = verts[2];
    Vec3 e1 = vec3_sub(p2, p1);
    Vec3 e2 = vec3_sub(p3, p1);
    Vec3 q = vec3_cross(ray.direction, e2);
    float a = vec3_dot(e1, q);
    if(a > -EPSILON && a < EPSILON) {
        return;
    }

    float f = 1/a;
    Vec3 s = vec3_sub(ray.origin, p1);
    float u = f * vec3_dot(s, q);

    if(u < 0) {
        return;
    }

    Vec3 r = vec3_cross(s, e1);
    float v = f * vec3_dot(ray.direction, r);

    if(v < 0 || u + v > 1) {
        return;
    }

    float t = f * vec3_dot(e2, r);

    if(t < 0 || t > info->hitDistance) {
        return;
    }

    if(vec3_dot(ray.direction, vec3_cross(e1, e2)) > 0) {
        return;
    }

    info->hasHit = 1;
    info->hitDistance = t;
    info->hitPosition = ray_hit_position(ray, t);
    info->material = mat;
    
    Vec2 uvA = uvs[0];
    Vec2 uvB = uvs[1];
    Vec2 uvC = uvs[2];

    Vec3 normA = norms[0];
    Vec3 normB = norms[1];
    Vec3 normC = norms[2];

    Vec3 v0 = vec3_sub(p2, p1);
    Vec3 v1 = vec3_sub(p3, p1);
    Vec3 v2 = vec3_sub(info->hitPosition, p1);

    float d00 = vec3_dot(v0, v0);
    float d01 = vec3_dot(v0, v1);
    float d11 = vec3_dot(v1, v1);
    float d20 = vec3_dot(v2, v0);
    float d21 = vec3_dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    float baryV = (d11 * d20 - d01 * d21) / denom;
    float baryW = (d00 * d21 - d01 * d20) / denom;
    float baryU = 1.0f - baryV - baryW;

    float texU = baryU * uvA.x + baryV * uvB.x + baryW * uvC.x;
    float texV = baryU * uvA.y + baryV * uvB.y + baryW * uvC.y;

    float normX = baryU * normA.x + baryV * normB.x + baryW * normC.x;
    float normY = baryU * normA.y + baryV * normB.y + baryW * normC.y;
    float normZ = baryU * normA.z + baryV * normB.z + baryW * normC.z;

    info->uv = vec2_build(texU, texV);
    info->normal = vec3_normalize(vec3_build(normX, normY, normZ));
}

void mesh_intersect(Model model, Ray ray, HitInfo* info) {
    Mesh mesh = model.mesh;
    for(int i = 0; i < mesh.faceCount; i++) {
        Face face = mesh.faces[i];
        Vec3* verts = malloc(3 * sizeof(Vec3));
        Vec3* norms = malloc(3 * sizeof(Vec3));
        Vec2* uvs = malloc(3 * sizeof(Vec2));
        verts[0] = mesh.vertices[face.v[0]];
        verts[1] = mesh.vertices[face.v[1]];
        verts[2] = mesh.vertices[face.v[2]];
        norms[0] = mesh.normals[face.vn[0]];
        norms[1] = mesh.normals[face.vn[1]];
        norms[2] = mesh.normals[face.vn[2]];
        uvs[0] = mesh.uvs[face.vt[0]];
        uvs[1] = mesh.uvs[face.vt[1]];
        uvs[2] = mesh.uvs[face.vt[2]];

        face_intersect(verts, norms, uvs, ray, info, model.material);

        free(verts);
        free(norms);
        free(uvs);
    }
}

void freeMesh(Mesh *mesh) {
    free(mesh->vertices);
    free(mesh->normals);
    free(mesh->uvs);
    free(mesh->faces);
}

#endif /* GEOMETR_H */