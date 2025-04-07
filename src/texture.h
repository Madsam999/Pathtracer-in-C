#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    int width;
    int height;
    Pixel* texture;
} Texture;

void skip_whitespace_and_comments(FILE* fp) {
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (isspace(c)) continue;
        if (c == '#') {
            // Skip the rest of the line
            while ((c = fgetc(fp)) != '\n' && c != EOF);
        } else {
            ungetc(c, fp);
            break;
        }
    }
}

Texture loadTexture(const char* filename) {
    Texture tex = {0};
    FILE* fp = fopen(filename, "rb");
    if(!fp) {
        perror("Failed to open file");
        return tex;
    }

    char format[3];
    int n = fscanf(fp, "%2s", format);
    if (n != 1 || strcmp(format, "P6") != 0) {
        fprintf(stderr, "Unsupported format or failed to read magic number: got '%s'\n", format);
        fclose(fp);
        return tex;
    }

    int width = 0, height = 0, max_color = 0;

    skip_whitespace_and_comments(fp);
    fscanf(fp, "%d", &width);

    skip_whitespace_and_comments(fp);
    fscanf(fp, "%d", &height);

    skip_whitespace_and_comments(fp);
    fscanf(fp, "%d", &max_color);

    if(max_color != 255) {
        fprintf(stderr, "Only max color 255 supported (got %d)\n", max_color);
        fclose(fp);
        return tex;
    }

    fgetc(fp);

    Pixel* pixels = malloc(width * height * sizeof(Pixel));
    if(!pixels) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(fp);
        return tex;
    }

    size_t read = fread(pixels, sizeof(Pixel), width * height, fp);
    if(read != width * height) {
        fprintf(stderr, "Failed to read all pixel data (expected %d, got %zu)\n",
            width * height, read);
        free(pixels);
        fclose(fp);
        return tex;
    }

    fclose(fp);
    
    tex.height = height;
    tex.width = width;
    tex.texture = pixels;
    return tex;
}

void freeTexture(Texture* tex) {
    free(tex->texture);
}

#endif /* TEXTURE_H */