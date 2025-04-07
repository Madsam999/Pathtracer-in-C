#include <stdio.h>

// Function to write a PPM file
void writePPM(const char *filename, int width, int height, const unsigned char *pixelData) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    // Write the header
    fprintf(file, "P6\n"); // P6 indicates a binary PPM file
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "255\n"); // Maximum color value

    // Write the pixel data
    fwrite(pixelData, 1, width * height * 3, file);

    fclose(file);
}