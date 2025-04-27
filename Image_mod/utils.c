#include "utils.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// [Part 2.4.1 Implementation] Reads raw data using fseek and fread. Basic error check.
void file_rawRead(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    if (!file || !buffer) return;
    if (fseek(file, position, SEEK_SET) != 0) {
        fprintf(stderr, "Error: fseek failed to position %u.\n", position);
        return;
    }
    if (fread(buffer, size, n, file) != n) {
        fprintf(stderr, "Error: fread failed to read %zu elements of size %u at position %u.\n", n, size, position);
         if(ferror(file)) {
            perror("fread error");
         } else if (feof(file)) {
             fprintf(stderr, "fread error: unexpected end of file.\n");
         }
    }
}

// [Part 2.4.1 Implementation] Writes raw data using fseek and fwrite.
void file_rawWrite(uint32_t position, void *buffer, uint32_t size, size_t n, FILE *file) {
    if (!file || !buffer) return;
    if (fseek(file, position, SEEK_SET) != 0) {
        fprintf(stderr, "Error: fseek failed to position %u for writing.\n", position);
        return;
    }
    if (fwrite(buffer, size, n, file) != n) {
        fprintf(stderr, "Error: fwrite failed to write %zu elements of size %u at position %u.\n", n, size, position);
         if(ferror(file)) {
            perror("fwrite error");
         }
    }
}

float** allocate_kernel(int size) {
    if (size <= 0) return NULL;
    float **kernel = (float **)malloc(size * sizeof(float *));
    if (!kernel) return NULL;
    kernel[0] = (float *)calloc(size * size, sizeof(float));
    if (!kernel[0]) {
        free(kernel);
        return NULL;
    }
    for (int i = 1; i < size; ++i) {
        kernel[i] = kernel[0] + i * size;
    }
    return kernel;
}

void free_kernel(float **kernel, int size) {
    if (kernel) {
        if(kernel[0]) free(kernel[0]);
        free(kernel);
    }
    (void)size;
}

// [Part 3.4.1 Implementation] Converts RGB to YUV using formula 3.3.
t_yuv rgb_to_yuv(t_pixel rgb) {
    t_yuv yuv;
    double r = rgb.red;
    double g = rgb.green;
    double b = rgb.blue;

    yuv.y =  0.299 * r + 0.587 * g + 0.114 * b;
    yuv.u = -0.14713 * r - 0.28886 * g + 0.436 * b;
    yuv.v =  0.615 * r - 0.51499 * g - 0.10001 * b;

    return yuv;
}

uint8_t clamp_u8(double value) {
    value = round(value);
    if (value < 0.0) return 0;
    if (value > 255.0) return 255;
    return (uint8_t)value;
}

// [Part 3.4.1 Implementation] Converts YUV to RGB using formula 3.4, includes rounding and clamping.
t_pixel yuv_to_rgb(t_yuv yuv) {
    t_pixel rgb;
    double y = yuv.y;
    double u = yuv.u;
    double v = yuv.v;

    double r = y + 1.13983 * v;
    double g = y - 0.39465 * u - 0.58060 * v;
    double b = y + 2.03211 * u;

    rgb.red   = clamp_u8(r);
    rgb.green = clamp_u8(g);
    rgb.blue  = clamp_u8(b);

    return rgb;
}

int calculate_row_stride(int width) {
    int bytes_per_row = width * 3;
    return (bytes_per_row + 3) & ~3;
}