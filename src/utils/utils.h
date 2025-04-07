#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <stdlib.h>

#pragma once

#define PI 3.1415926535897932384626433

float deg2rad(float angle) {
    return angle * PI/180;
}

float random01() {
    return (double)rand() / (double)RAND_MAX;
}

float random_range(float min, float max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

#endif /* UTILS_H */