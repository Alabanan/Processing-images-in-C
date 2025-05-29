#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdint.h>
#include "bmp24.h"

// [Part 2.4.1 step 1] Function file_rawRead Reads raw bytes from a specific file position.
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);
// [Part 2.4.1 step 2] Function file_rawWrite Writes raw bytes to a specific file position.
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file);

float** allocate_kernel(int size);
void free_kernel(float **kernel, int size);

// [Part 3.4.1 step 1] Defines a structure to hold YUV values (using double for precision during conversion).
typedef struct {
    double y;
    double u;
    double v;
} t_yuv;

// [Part 3.4.1 step 2] Function rgb_to_yuv converts an RGB pixel to YUV color space.
t_yuv rgb_to_yuv(t_pixel rgb);
// [Part 3.4.1 step 3] Function yuv_to_rgb converts a YUV value back to an RGB pixel, performing clamping and rounding.
t_pixel yuv_to_rgb(t_yuv yuv);

uint8_t clamp_u8(double value);
int calculate_row_stride(int width);


#endif // UTILS_H