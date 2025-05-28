#ifndef BMP24_H
#define BMP24_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


// [Part 2.2.2] Defines the structure for a single 24-bit pixel (BGR order).
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} t_pixel;

// [Part 2.2.1] Defines the BMP file header structure (14 bytes).
#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} t_bmp_header;
#pragma pack(pop)

// [Part 2.2.1] Defines the BMP info header structure (BITMAPINFOHEADER, 40 bytes).
#pragma pack(push, 1)
typedef struct {
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t imagesize;
    int32_t  xresolution;
    int32_t  yresolution;
    uint32_t ncolors;
    uint32_t importantcolors;
} t_bmp_info;
#pragma pack(pop)

// [Part 2.2] Defines the main structure for a 24-bit BMP image.
typedef struct {
    t_bmp_header header;
    t_bmp_info   header_info;
    int width;
    int height;
    int colorDepth;
    t_pixel **data;
} t_bmp24;

// [Part 2.2.3] Useful constants for BMP format.
#define BITMAP_MAGIC        0x4D42
#define BMP_HEADER_SIZE     14
#define BMP_INFOHEADER_SIZE 40
#define DEFAULT_OFFSET      (BMP_HEADER_SIZE + BMP_INFOHEADER_SIZE)
#define DEFAULT_DEPTH       24
#define NO_COMPRESSION      0



t_pixel **bmp24_allocateDataPixels(int width, int height);
void bmp24_freeDataPixels(t_pixel **pixels, int height);
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth);
void bmp24_free(t_bmp24 *img);

t_bmp24 *bmp24_loadImage(const char *filename);
void bmp24_saveImage(const char *filename, t_bmp24 *img);
void bmp24_printInfo(t_bmp24 *img);

int bmp24_readPixelData(t_bmp24 *img, FILE *file);
int bmp24_writePixelData(t_bmp24 *img, FILE *file);

void bmp24_negative(t_bmp24 *img);
void bmp24_grayscale(t_bmp24 *img);
void bmp24_brightness(t_bmp24 *img, int value);

t_pixel bmp24_convolution_helper(t_pixel **original_data, int x, int y, int width, int height, float **kernel, int kernelSize);
void bmp24_applyFilter(t_bmp24 *img, float **kernel, int kernelSize);

void bmp24_boxBlur(t_bmp24 *img);
void bmp24_gaussianBlur(t_bmp24 *img);
void bmp24_outline(t_bmp24 *img);
void bmp24_emboss(t_bmp24 *img);
void bmp24_sharpen(t_bmp24 *img);



void bmp24_equalize(t_bmp24 *img);


#endif // BMP24_H