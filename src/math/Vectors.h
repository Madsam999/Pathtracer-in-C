#ifndef VECTORS_H
#define VECTORS_H

#pragma once

#include <math.h>
#include <stdio.h>

#include "../utils/utils.h"

typedef struct Vec2
{
    float x;
    float y;
} Vec2, UV;


typedef struct Vec3
{
    float x;
    float y;
    float z;
} Vec3, RGB;

typedef struct Vec4
{
    float x;
    float y;
    float z;
    float w;
} Vec4, RGBA;

Vec2 vec2_build(float x, float y) {
    Vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

void vec3_print(Vec3 v) {
    printf("(%.3f, %.3f, %.3f)\n", v.x, v.y, v.z);
}

Vec3 vec3_build(float x, float y, float z) {
    Vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

Vec3 vec3_add(Vec3 v1, Vec3 v2) {
    Vec3 v3;
    v3.x = v1.x + v2.x;
    v3.y = v1.y + v2.y;
    v3.z = v1.z + v2.z;
    return v3;
}

Vec3 vec3_sub(Vec3 v1, Vec3 v2) {
    Vec3 v3;
    v3.x = v1.x - v2.x;
    v3.y = v1.y - v2.y;
    v3.z = v1.z - v2.z;
    return v3;
}

Vec3 vec3_mul(Vec3 vec, float scalar) {
    Vec3 v;
    v.x = vec.x * scalar;
    v.y = vec.y * scalar;
    v.z = vec.z * scalar;
    return v;
}

Vec3 vec3_vec3_mul(Vec3 v, Vec3 u) {
    Vec3 w;
    w.x = v.x * u.x;
    w.y = v.y * u.y;
    w.z = v.z * u.z;
    return w;
}

Vec3 vec3_div(Vec3 vec, float scalar) {
    Vec3 v;
    float inv = 1/scalar;
    v.x = vec.x * inv;
    v.y = vec.y * inv;
    v.z = vec.z * inv;
    return v;
}

float vec3_length(Vec3 v) {
    float norm = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
    return norm;
}

Vec3 vec3_normalize(Vec3 v) {
    Vec3 normalized;
    float norm = vec3_length(v);
    float normInverse = 1/norm;
    normalized.x = v.x * normInverse;
    normalized.y = v.y * normInverse;
    normalized.z = v.z * normInverse;
    return normalized;
}

void vec3_clamp(Vec3 min, Vec3 max, Vec3* val) {
    if(val->x < min.x) {
        val->x = min.x;
    }
    else if(val->x > max.x) {
        val->x = max.x;
    }
    if(val->y < min.y) {
        val->y = min.y;
    }
    else if(val->y > max.y) {
        val->y = max.y;
    }
    if(val->z < min.z) {
        val->z = min.z;
    }
    else if(val->z > max.z) {
        val->z = max.z;
    }
}

float vec3_dot(Vec3 v1, Vec3 v2) {
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z); 
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    Vec3 v;
    v.x = (a.y * b.z) - (a.z * b.y);
    v.y = (a.z * b.x) - (a.x * b.z);
    v.z = (a.x * b.y) - (a.y * b.x);
    return v;
}

Vec3 vec3_reflect(Vec3 v, Vec3 n) {
    Vec3 w;
    float dot = vec3_dot(v, n);
    w = vec3_sub(v, vec3_mul(n, 2 * dot));
    return w;
}

Vec3 random_vec301() {
    return vec3_build(random01(), random01(), random01());
}

Vec3 random_vec3_range(float min, float max) {
    return vec3_build(random_range(min, max), random_range(min, max), random_range(min, max));
}

Vec3 random_unit_vector() {
    while(1) {
        Vec3 p = random_vec3_range(-1, 1);
        float lensq = pow(vec3_length(p), 2);
        if(1e-160 < lensq && lensq <= 1) {
            return vec3_normalize(p);
        }
    }
}

Vec3 random_on_hemisphere(Vec3 normal) {
    Vec3 on_unit_sphere = random_unit_vector();
    if(vec3_dot(on_unit_sphere, normal) > 0.0f) {
        return on_unit_sphere;
    }
    else {
        return vec3_mul(on_unit_sphere, -1.0f);
    }
}

Vec3 vec3_lerp(Vec3 v1, Vec3 v2, float t) {
    Vec3 u = vec3_mul(v1, 1 - t);
    Vec3 v = vec3_mul(v2, t);
    return vec3_add(u, v);
}


Vec4 vec4_build(float x, float y, float z, float w) {
    Vec4 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    vec.w = w;
    return vec;
}

Vec4 vec4_build_from_vec3(Vec3 v, float w) {
    Vec4 vec;
    vec.x = v.x;
    vec.y = v.y;
    vec.z = v.z;
    vec.w = w;
    return vec;
}

Vec4 vec4_mat4_mult(Vec4 v, float* mat) {
    Vec4 result;
    result.x = v.x * mat[0] + v.y * mat[1 + 4 * 0] + v.z * mat[2 + 4 * 0] + v.w * mat[3 + 4 * 0];
    result.y = v.x * mat[0 + 4 * 1] + v.y * mat[1 + 4 * 1] + v.z * mat[2 + 4 * 1] + v.w * mat[3 + 4 * 1];
    result.z = v.x * mat[0 + 4 * 2] + v.y * mat[1 + 4 * 2] + v.z * mat[2 + 4 * 2] + v.w * mat[3 + 4 * 2];
    result.w = v.x * mat[0 + 4 * 3] + v.y * mat[1 + 4 * 3] + v.z * mat[2 + 4 * 3] + v.w * mat[3 + 4 * 3];
    return result;
}

void vec4_print(Vec4 v) {
    printf("(%f, %f, %f, %f)\n", v.x, v.y, v.z, v.w);
}

#endif /* VECTORS_H */