#ifndef BMP8_H
#define BMP8_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// [Part 1.1] Defines the structure for an 8-bit BMP image.
typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char *data;

    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
} t_bmp8;

// [Part 1.2.1] Function bmp8_loadImage is needed to read an 8-bit BMP file into memory.
t_bmp8 *bmp8_loadImage(const char *filename);

// [Part 1.2.2] Function bmp8_saveImage is needed to write an 8-bit BMP image from memory to a file.
void bmp8_saveImage(const char *filename, t_bmp8 *img);

// [Part 1.2.3] Function bmp8_free is needed to release memory allocated for an 8-bit BMP image.
void bmp8_free(t_bmp8 *img);

// [Part 1.2.4] Function bmp8_printInfo is needed to display metadata of the loaded 8-bit BMP image.
void bmp8_printInfo(t_bmp8 *img);

// [Part 1.3.1] Function bmp8_negative is needed to apply color inversion to an 8-bit image.
void bmp8_negative(t_bmp8 *img);

// [Part 1.3.2] Function bmp8_brightness is needed to adjust the brightness of an 8-bit image.
void bmp8_brightness(t_bmp8 *img, int value);

// [Part 1.3.3] Function bmp8_threshold is needed to convert an 8-bit image to black and white based on a threshold.
void bmp8_threshold(t_bmp8 *img, int threshold);

// [Part 1.4.1 step 1] Function bmp8_applyFilter is needed to apply a generic convolution filter (kernel) to an 8-bit image.
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);

void bmp8_boxBlur(t_bmp8 *img);
void bmp8_gaussianBlur(t_bmp8 *img);
void bmp8_outline(t_bmp8 *img);
void bmp8_emboss(t_bmp8 *img);
void bmp8_sharpen(t_bmp8 *img);


// [Part 3.3.1 step 1] Function bmp8_computeHistogram is needed to calculate the frequency of each gray level in an 8-bit image.
unsigned int *bmp8_computeHistogram(t_bmp8 *img);

// [Part 3.3.2 step 1] Function bmp8_computeCDF is needed to calculate the normalized cumulative distribution function from a histogram.

unsigned int *bmp8_computeCDF(unsigned int *hist, int numPixels);

// [Part 3.3.3 step 1] Function bmp8_equalize is needed to apply histogram equalization to enhance the contrast of an 8-bit image.
void bmp8_equalize(t_bmp8 *img);


#endif // BMP8_H