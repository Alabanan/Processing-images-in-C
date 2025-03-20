#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

t_bmp8 * bmp8_loadImage(const char * filename) {
    t_bmp8 * bmp = (t_bmp8 *) malloc(sizeof(t_bmp8));
    if (bmp == NULL) {
        perror("Error allocating memory");
        return NULL;
    }

    FILE * file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        free(bmp);
        return NULL;
    }

    fread(bmp->header, 1, 54, file);
    bmp->width = *(int*)&bmp->header[18];
    bmp->height = *(int*)&bmp->header[22];
    bmp->colorDepth = *(int*)&bmp->header[28];
    bmp->dataSize = *(int*)&bmp->header[34];

    if (bmp->colorDepth != 8) {
        fprintf(stderr, "Not an 8-bit image\n");
        fclose(file);
        free(bmp);
        return NULL;
    }

    fread(bmp->colorTable, 1, 1024, file);

    bmp->data = (unsigned char *)malloc(bmp->dataSize);
    if (bmp->data == NULL) {
        perror("Error allocating memory");
        fclose(file);
        free(bmp);
        return NULL;
    }
    fread(bmp->data, 1, bmp->dataSize, file);

    fclose(file);
    return bmp;
}

void bmp8_saveImage(const char * filename, t_bmp8 * img) {
    FILE * file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fwrite(img->header, 1, 54, file);
    fwrite(img->colorTable, 1, 1024, file);
    fwrite(img->data, 1, img->dataSize, file);

    if (fclose(file) != 0) {
        perror("Error closing file");
    }
}

void bmp8_free(t_bmp8 * img) {
    free(img->data);
}

void bmp8_printInfo(t_bmp8 * img) {
    printf("Width: %d\n", img->width);
    printf("Height: %d\n", img->height);
    printf("Color depth: %d\n", img->colorDepth);
    printf("Data size: %d\n", img->dataSize);
}

void bmp8_negative(t_bmp8 * img) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }
}

void bmp8_brightness(t_bmp8 * img, int value) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        int newValue = img->data[i] + value;
        if (newValue > 255) {
            newValue = 255;
        } else if (newValue < 0) {
            newValue = 0;
        }
        img->data[i] = (unsigned char)newValue;
    }
}

void bmp8_threshold(t_bmp8 * img, int threshold) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
    }
}

void bmp8_applyFilter(t_bmp8 * img, int * kernel, int kernelSize, int kernelSum) {
    unsigned char * newData = (unsigned char *)malloc(img->dataSize);
    for (unsigned int i = 0; i < img->height; i++) {
        for (unsigned int j = 0; j < img->width; j++) {
            int sum = 0;
            for (int k = 0; k < kernelSize; k++) {
                for (int l = 0; l < kernelSize; l++) {
                    int x = i + k - kernelSize / 2;
                    int y = j + l - kernelSize / 2;
                    if (x >= 0 && x < img->height && y >= 0 && y < img->width) {
                        sum += img->data[x * img->width + y] * kernel[k * kernelSize + l];
                    }
                }
            }
            newData[i * img->width + j] = (unsigned char)(sum / kernelSum);
        }
    }
    free(img->data);
    img->data = newData;
}