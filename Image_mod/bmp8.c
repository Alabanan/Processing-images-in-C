#include "bmp8.h"
#include "utils.h"
#include <math.h>

#define WIDTH_OFFSET 18
#define HEIGHT_OFFSET 22
#define DEPTH_OFFSET 28
#define DATA_SIZE_OFFSET 34
#define DATA_OFFSET_HDR 10
#define HEADER_SIZE 54
#define COLOR_TABLE_SIZE 1024

// [Part 1.2.1 Implementation] Reads BMP file, allocates memory, populates t_bmp8 struct.
t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }

    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (!img) {
        fprintf(stderr, "Error: Cannot allocate memory for image structure.\n");
        fclose(file);
        return NULL;
    }
    img->data = NULL;

    if (fread(img->header, 1, HEADER_SIZE, file) != HEADER_SIZE) {
        fprintf(stderr, "Error: Failed to read BMP header from %s.\n", filename);
        fclose(file);
        bmp8_free(img);
        return NULL;
    }

    img->width = *(unsigned int *)&img->header[WIDTH_OFFSET];
    img->height = *(unsigned int *)&img->header[HEIGHT_OFFSET];
    img->colorDepth = *(unsigned short *)&img->header[DEPTH_OFFSET];
    unsigned int dataOffset = *(unsigned int*)&img->header[DATA_OFFSET_HDR];

    unsigned int headerDataSize = *(unsigned int *)&img->header[DATA_SIZE_OFFSET];
    img->dataSize = img->width * img->height; // For 8-bit uncompressed
    if (headerDataSize != 0 && headerDataSize != img->dataSize) {
         printf("Warning: Header data size (%u) differs from calculated (%u)\n", headerDataSize, img->dataSize);
    }

    if (img->header[0] != 'B' || img->header[1] != 'M') {
        fprintf(stderr, "Error: File %s is not a valid BMP file (Invalid signature).\n", filename);
        fclose(file);
        bmp8_free(img);
        return NULL;
    }
    if (img->colorDepth != 8) {
        fprintf(stderr, "Error: Image %s is not an 8-bit grayscale image (colorDepth=%u).\n", filename, img->colorDepth);
        fclose(file);
        bmp8_free(img);
        return NULL;
    }
    if (dataOffset < HEADER_SIZE + COLOR_TABLE_SIZE) {
        fprintf(stderr, "Warning: BMP data offset (%u) seems incorrect, expected at least %d.\n", dataOffset, HEADER_SIZE + COLOR_TABLE_SIZE);
    }

    if (fread(img->colorTable, 1, COLOR_TABLE_SIZE, file) != COLOR_TABLE_SIZE) {
        fprintf(stderr, "Error: Failed to read color table from %s.\n", filename);
        fclose(file);
        bmp8_free(img);
        return NULL;
    }

    img->data = (unsigned char *)malloc(img->dataSize);
    if (!img->data) {
        fprintf(stderr, "Error: Cannot allocate memory for pixel data (%u bytes).\n", img->dataSize);
        fclose(file);
        bmp8_free(img);
        return NULL;
    }

    if (fseek(file, dataOffset, SEEK_SET) != 0) {
         fprintf(stderr, "Error: Failed to seek to pixel data offset (%u) in %s.\n", dataOffset, filename);
        fclose(file);
        bmp8_free(img);
        return NULL;
    }
    if (fread(img->data, 1, img->dataSize, file) != img->dataSize) {
        fprintf(stderr, "Error: Failed to read pixel data from %s.\n", filename);
         if(ferror(file)) perror("fread error"); else if(feof(file)) fprintf(stderr,"fread error: unexpected EOF\n");
        fclose(file);
        bmp8_free(img);
        return NULL;
    }

    fclose(file);
    printf("Image '%s' loaded successfully (%ux%u, %u-bit).\n", filename, img->width, img->height, img->colorDepth);
    return img;
}

// [Part 1.2.2 Implementation] Writes the t_bmp8 struct data back to a BMP file.
void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    if (!img || !img->data) {
        fprintf(stderr, "Error: Cannot save NULL or invalid image.\n");
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s for writing.\n", filename);
        return;
    }

    *(unsigned int*)&img->header[DATA_OFFSET_HDR] = HEADER_SIZE + COLOR_TABLE_SIZE;
    *(unsigned int*)&img->header[DATA_SIZE_OFFSET] = img->dataSize;
    *(unsigned int*)&img->header[2] = HEADER_SIZE + COLOR_TABLE_SIZE + img->dataSize;


    if (fwrite(img->header, 1, HEADER_SIZE, file) != HEADER_SIZE) {
        fprintf(stderr, "Error: Failed to write BMP header to %s.\n", filename);
        fclose(file);
        return;
    }

    if (fwrite(img->colorTable, 1, COLOR_TABLE_SIZE, file) != COLOR_TABLE_SIZE) {
        fprintf(stderr, "Error: Failed to write color table to %s.\n", filename);
        fclose(file);
        return;
    }

    if (fwrite(img->data, 1, img->dataSize, file) != img->dataSize) {
        fprintf(stderr, "Error: Failed to write pixel data to %s.\n", filename);
        fclose(file);
        return;
    }

    fclose(file);
    printf("Image saved successfully as %s.\n", filename);
}

// [Part 1.2.3 Implementation] Frees allocated memory.
void bmp8_free(t_bmp8 *img) {
    if (img) {
        if (img->data) {
            free(img->data);
            img->data = NULL;
        }
        free(img);
    }
}

// [Part 1.2.4 Implementation] Prints image info.
void bmp8_printInfo(t_bmp8 *img) {
    if (!img) {
        printf("Image Info: No 8-bit image loaded.\n");
        return;
    }
    printf("--- 8-bit Image Info ---\n");
    printf("  Width:       %u\n", img->width);
    printf("  Height:      %u\n", img->height);
    printf("  Color Depth: %u\n", img->colorDepth);
    printf("  Data Size:   %u bytes\n", img->dataSize);
    printf("  File Size (Header): %u bytes\n", *(unsigned int*)&img->header[2]);
    printf("  Data Offset (Header): %u \n", *(unsigned int*)&img->header[10]);
}


// [Part 1.3.1 Implementation] Inverts pixel values.
void bmp8_negative(t_bmp8 *img) {
    if (!img || !img->data) return;
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        img->data[i] = 255 - img->data[i];
    }
     printf("Negative filter applied (8-bit).\n");
}

// [Part 1.3.2 Implementation] Adjusts brightness, clamping values.
void bmp8_brightness(t_bmp8 *img, int value) {
     if (!img || !img->data) return;
    for (unsigned int i = 0; i < img->dataSize; ++i) {
        int newValue = (int)img->data[i] + value;
        if (newValue < 0) newValue = 0;
        if (newValue > 255) newValue = 255;
        img->data[i] = (unsigned char)newValue;
    }
    printf("Brightness adjusted by %d (8-bit).\n", value);
}

// [Part 1.3.3 Implementation] Applies thresholding.
void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (!img || !img->data) return;
    if (threshold < 0) threshold = 0;
    if (threshold > 255) threshold = 255;

    for (unsigned int i = 0; i < img->dataSize; ++i) {
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
    }
     printf("Threshold filter applied at %d (8-bit).\n", threshold);
}

// [Part 1.4.1 Implementation] Applies convolution filter.
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (!img || !img->data || !kernel || kernelSize <= 0 || kernelSize % 2 == 0) {
        fprintf(stderr, "Error: Invalid arguments for applyFilter (8-bit).\n");
        return;
    }

    unsigned int width = img->width;
    unsigned int height = img->height;
    unsigned int dataSize = img->dataSize;
    int n = kernelSize / 2;

    unsigned char *tempData = (unsigned char *)malloc(dataSize);
    if (!tempData) {
        fprintf(stderr, "Error: Failed to allocate memory for temp data in filter (8-bit).\n");
        return;
    }
    memcpy(tempData, img->data, dataSize);

    for (unsigned int y = n; y < height - n; ++y) {
        for (unsigned int x = n; x < width - n; ++x) {
            float sum = 0.0f;

            for (int ky = -n; ky <= n; ++ky) { // Loop from -n to n for kernel indices
                for (int kx = -n; kx <= n; ++kx) {
                    unsigned int pixelY = y + ky;
                    unsigned int pixelX = x + kx;
                    unsigned int pixelIndex = pixelY * width + pixelX;
                    sum += tempData[pixelIndex] * kernel[ky + n][kx + n];
                }
            }

            if (sum < 0.0f) sum = 0.0f;
            if (sum > 255.0f) sum = 255.0f;

            img->data[y * width + x] = (unsigned char)round(sum);
        }
    }

    free(tempData);
    printf("Applied %dx%d filter (8-bit).\n", kernelSize, kernelSize);
}

void bmp8_boxBlur(t_bmp8 *img) {
    int size = 3;
    float **kernel = allocate_kernel(size);
    if (!kernel) { fprintf(stderr,"Kernel alloc failed for box blur\n"); return; }
    float val = 1.0f / 9.0f;
    for(int i=0; i<size; ++i) for(int j=0; j<size; ++j) kernel[i][j] = val;
    bmp8_applyFilter(img, kernel, size);
    free_kernel(kernel, size);
}

void bmp8_gaussianBlur(t_bmp8 *img) {
    int size = 3;
    float **kernel = allocate_kernel(size);
    if (!kernel) { fprintf(stderr,"Kernel alloc failed for gaussian blur\n"); return; }
    kernel[0][0] = 1.0f/16.0f; kernel[0][1] = 2.0f/16.0f; kernel[0][2] = 1.0f/16.0f;
    kernel[1][0] = 2.0f/16.0f; kernel[1][1] = 4.0f/16.0f; kernel[1][2] = 2.0f/16.0f;
    kernel[2][0] = 1.0f/16.0f; kernel[2][1] = 2.0f/16.0f; kernel[2][2] = 1.0f/16.0f;
    bmp8_applyFilter(img, kernel, size);
    free_kernel(kernel, size);
}
void bmp8_outline(t_bmp8 *img) {
    int size = 3;
    float **kernel = allocate_kernel(size);
     if (!kernel) { fprintf(stderr,"Kernel alloc failed for outline\n"); return; }
    kernel[0][0] = -1.0f; kernel[0][1] = -1.0f; kernel[0][2] = -1.0f;
    kernel[1][0] = -1.0f; kernel[1][1] =  8.0f; kernel[1][2] = -1.0f;
    kernel[2][0] = -1.0f; kernel[2][1] = -1.0f; kernel[2][2] = -1.0f;
    bmp8_applyFilter(img, kernel, size);
    free_kernel(kernel, size);
}

void bmp8_emboss(t_bmp8 *img) {
    int size = 3;
    float **kernel = allocate_kernel(size);
     if (!kernel) { fprintf(stderr,"Kernel alloc failed for emboss\n"); return; }
    kernel[0][0] = -2.0f; kernel[0][1] = -1.0f; kernel[0][2] =  0.0f;
    kernel[1][0] = -1.0f; kernel[1][1] =  1.0f; kernel[1][2] =  1.0f;
    kernel[2][0] =  0.0f; kernel[2][1] =  1.0f; kernel[2][2] =  2.0f;
    bmp8_applyFilter(img, kernel, size);
    free_kernel(kernel, size);
}

void bmp8_sharpen(t_bmp8 *img) {
     int size = 3;
    float **kernel = allocate_kernel(size);
     if (!kernel) { fprintf(stderr,"Kernel alloc failed for sharpen\n"); return; }
    kernel[0][0] =  0.0f; kernel[0][1] = -1.0f; kernel[0][2] =  0.0f;
    kernel[1][0] = -1.0f; kernel[1][1] =  5.0f; kernel[1][2] = -1.0f;
    kernel[2][0] =  0.0f; kernel[2][1] = -1.0f; kernel[2][2] =  0.0f;
    bmp8_applyFilter(img, kernel, size);
    free_kernel(kernel, size);
}

// [Part 3.3.1 Implementation] Computes histogram.
unsigned int *bmp8_computeHistogram(t_bmp8 *img) {
    if (!img || !img->data) return NULL;

    unsigned int *hist = (unsigned int *)calloc(256, sizeof(unsigned int));
    if (!hist) {
        fprintf(stderr, "Error: Failed to allocate memory for histogram (8-bit).\n");
        return NULL;
    }

    for (unsigned int i = 0; i < img->dataSize; ++i) {
        hist[img->data[i]]++;
    }
    return hist;
}


// [Part 3.3.2 Implementation] Computes normalized CDF (mapping table).
unsigned int *bmp8_computeCDF(unsigned int *hist, int numPixels) {
     if (!hist || numPixels <= 0) return NULL;

    unsigned int *cdf = (unsigned int *)calloc(256, sizeof(unsigned int));
    unsigned int *hist_eq = (unsigned int *)calloc(256, sizeof(unsigned int)); // mapping table
    if (!cdf || !hist_eq) {
        fprintf(stderr, "Error: Failed to allocate memory for CDF/HistEq (8-bit).\n");
        free(cdf);
        free(hist_eq);
        return NULL;
    }

    cdf[0] = hist[0];
    for (int i = 1; i < 256; ++i) {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    unsigned int cdf_min = 0;
    int min_gray_level = -1;
    for(int i=0; i<256; ++i) {
        if(hist[i] > 0) {
            min_gray_level = i;
            break;
        }
    }
    if(min_gray_level != -1) {
        cdf_min = cdf[min_gray_level];
    } else {
        cdf_min = 0;
         fprintf(stderr, "Warning: Could not find minimum non-zero CDF value. Equalization might be incorrect.\n");
    }


    double denominator = (double)numPixels - cdf_min;
    if (denominator <= 0) {
        fprintf(stderr,"Warning: Cannot normalize histogram (numPixels=%d, cdf_min=%u). Mapping gray levels linearly.\n", numPixels, cdf_min);
        for(int i=0; i<256; ++i) hist_eq[i] = i;
    } else {
        for (int i = 0; i < 256; ++i) {
            if (cdf[i] < cdf_min) {
                 hist_eq[i] = 0;
            } else {
                 double numerator = (double)cdf[i] - cdf_min;
                 double mapped_value = round((numerator / denominator) * 255.0);


                 if (mapped_value < 0) mapped_value = 0;
                 if (mapped_value > 255) mapped_value = 255;
                 hist_eq[i] = (unsigned int)mapped_value;
            }
        }
    }

    free(cdf);
    return hist_eq;
}


// [Part 3.3.3 Implementation] Applies histogram equalization.
void bmp8_equalize(t_bmp8 *img) {
    if (!img || !img->data || img->dataSize == 0) return;

    unsigned int *hist = bmp8_computeHistogram(img);
    if (!hist) return;

    unsigned int *hist_eq = bmp8_computeCDF(hist, img->dataSize);
    if (!hist_eq) {
        free(hist);
        return;
    }


    for (unsigned int i = 0; i < img->dataSize; ++i) {
        img->data[i] = hist_eq[img->data[i]];
    }

    printf("Histogram equalization applied (8-bit).\n");


    free(hist);
    free(hist_eq);
}