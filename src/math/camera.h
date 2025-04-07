#ifndef CAMERA_H
#define CAMERA_H

#pragma once

#include "Vectors.h"

typedef struct Camera {
    float fov;
    Vec3 position;
    Vec3 target;
    Vec3 up;
    float near;
    float far;
    float aspectRatio;
} Camera;

Camera camera_create(float fov, Vec3 position, Vec3 target, Vec3 up, float near, float far, float aspectRatio) {
    Camera cam;
    cam.fov = fov;
    cam.position = position;
    cam.target = target;
    cam.up = up;
    cam.near = near;
    cam.far = far;
    cam.aspectRatio = aspectRatio;
    return cam;
}

void computeCamToWorld(Camera* cam, float* matrix) {
    Vec3 f = vec3_normalize(vec3_sub(cam->target, cam->position));
    Vec3 u = vec3_normalize(cam->up);
    Vec3 s = vec3_normalize(vec3_cross(f, u));
    u = vec3_cross(s, f);

    matrix[0 + 4 * 0] = s.x;
    matrix[1 + 4 * 0] = s.y;
    matrix[2 + 4 * 0] = s.z;
    matrix[3 + 4 * 0] = 0.0f;

    matrix[0 + 4 * 1] = u.x;
    matrix[1 + 4 * 1] = u.y;
    matrix[2 + 4 * 1] = u.z;
    matrix[3 + 4 * 1] = 0.0f;

    matrix[0 + 4 * 2] = -f.x;
    matrix[1 + 4 * 2] = -f.y;
    matrix[2 + 4 * 2] = -f.z;
    matrix[3 + 4 * 2] = 0.0f;

    matrix[3 + 4 * 0] = -vec3_dot(s, cam->position);
    matrix[3 + 4 * 1] = -vec3_dot(u, cam->position);
    matrix[3 + 4 * 2] = vec3_dot(f, cam->position);
    matrix[3 + 4 * 3] = 1.0f;
}

float determinant(float *matrix) {
    return matrix[0] * (
        matrix[5] * (matrix[10] * matrix[15] - matrix[11] * matrix[14]) -
        matrix[6] * (matrix[9] * matrix[15] - matrix[11] * matrix[13]) +
        matrix[7] * (matrix[9] * matrix[14] - matrix[10] * matrix[13])
    ) - matrix[1] * (
        matrix[4] * (matrix[10] * matrix[15] - matrix[11] * matrix[14]) -
        matrix[6] * (matrix[8] * matrix[15] - matrix[11] * matrix[12]) +
        matrix[7] * (matrix[8] * matrix[14] - matrix[10] * matrix[12])
    ) + matrix[2] * (
        matrix[4] * (matrix[9] * matrix[15] - matrix[11] * matrix[13]) -
        matrix[5] * (matrix[8] * matrix[15] - matrix[11] * matrix[12]) +
        matrix[7] * (matrix[8] * matrix[13] - matrix[9] * matrix[12])
    ) - matrix[3] * (
        matrix[4] * (matrix[9] * matrix[14] - matrix[10] * matrix[13]) -
        matrix[5] * (matrix[8] * matrix[14] - matrix[10] * matrix[12]) +
        matrix[6] * (matrix[8] * matrix[13] - matrix[9] * matrix[12])
    );
}

void getCofactor(float *matrix, float *cofactor, int p, int q, int n) {
    int i = 0, j = 0;
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            if (row != p && col != q) {
                cofactor[i * n + j] = matrix[row * n + col];
                j++;
                if (j == n - 1) {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

void adjugate(float *matrix, float *adj) {
    float cofactor[9];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            getCofactor(matrix, cofactor, i, j, 4);
            adj[j * 4 + i] = ((i + j) % 2 == 0 ? 1 : -1) * determinant(cofactor);
        }
    }
}

// Function to invert a 4x4 matrix
void inverse(float *matrix, float *inverse) {
    float det = determinant(matrix);
    if (det == 0) {
        printf("Matrix is not invertible.\n");
        return;
    }

    float adj[16];
    adjugate(matrix, adj);

    for (int i = 0; i < 16; i++) {
        inverse[i] = adj[i] / det;
    }
}


#endif /* CAMERA_H */