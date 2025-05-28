#include "bmp24.h"
#include "bmp8.h"
#include "utils.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>


#define HEADER_TYPE_OFFSET 0
#define HEADER_SIZE_OFFSET 2
#define HEADER_OFFSET_OFFSET 10
#define INFO_SIZE_OFFSET BMP_HEADER_SIZE
#define INFO_WIDTH_OFFSET (BMP_HEADER_SIZE + 4)
#define INFO_HEIGHT_OFFSET (BMP_HEADER_SIZE + 8)
#define INFO_PLANES_OFFSET (BMP_HEADER_SIZE + 12)
#define INFO_BITS_OFFSET (BMP_HEADER_SIZE + 14)
#define INFO_COMPRESSION_OFFSET (BMP_HEADER_SIZE + 16)
#define INFO_IMAGESIZE_OFFSET (BMP_HEADER_SIZE + 20)
#define INFO_XRES_OFFSET (BMP_HEADER_SIZE + 24)
#define INFO_YRES_OFFSET (BMP_HEADER_SIZE + 28)
#define INFO_NCOLORS_OFFSET (BMP_HEADER_SIZE + 32)
#define INFO_IMPORTANTCOLORS_OFFSET (BMP_HEADER_SIZE + 36)


// [Part 2.3 Implementation] Allocate 2D pixel array
t_pixel **bmp24_allocateDataPixels(int width, int height) {
    if (width <= 0 || height <= 0) return NULL;

    t_pixel **pixels = (t_pixel **)malloc(height * sizeof(t_pixel *));
    if (!pixels) {
        fprintf(stderr, "Error: Failed to allocate memory for pixel rows.\n");
        return NULL;
    }
    pixels[0] = (t_pixel *)calloc(height * width, sizeof(t_pixel));
    if (!pixels[0]) {
        fprintf(stderr, "Error: Failed to allocate memory for pixel data block.\n");
        free(pixels);
        return NULL;
    }
    for (int i = 1; i < height; ++i) {
        pixels[i] = pixels[0] + i * width;
    }
    return pixels;
}

// [Part 2.3 Implementation] Free 2D pixel array (allocated contiguously)
void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (!pixels) return;
    if (pixels[0]) {
        free(pixels[0]);
    }
    free(pixels);
    (void)height;
}

// [Part 2.3 Implementation] Allocate t_bmp24 structure and its data
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
     if (width <= 0 || height <= 0) return NULL;

    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    if (!img) {
        fprintf(stderr, "Error: Failed to allocate memory for t_bmp24 structure.\n");
        return NULL;
    }

    img->data = bmp24_allocateDataPixels(width, height);
    if (!img->data) {
        free(img);
        return NULL;
    }

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;

    memset(&img->header, 0, sizeof(t_bmp_header));
    memset(&img->header_info, 0, sizeof(t_bmp_info));
    img->header.type = BITMAP_MAGIC;
    img->header.offset = DEFAULT_OFFSET; // 54
    img->header_info.size = BMP_INFOHEADER_SIZE; // 40
    img->header_info.width = width;
    img->header_info.height = height;
    img->header_info.planes = 1;
    img->header_info.bits = colorDepth; // Usually 24
    img->header_info.compression = NO_COMPRESSION;
    int row_stride = calculate_row_stride(width);
    img->header_info.imagesize = row_stride * height;
    img->header.size = img->header.offset + img->header_info.imagesize;

    return img;
}

// [Part 2.3 Implementation] Free the entire t_bmp24 structure
void bmp24_free(t_bmp24 *img) {
    if (img) {
        bmp24_freeDataPixels(img->data, img->height);
        img->data = NULL;
        free(img);
    }
}

// [Part 2.4.3 Implementation] Load 24-bit BMP
t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }

    t_bmp_header header;
    t_bmp_info info;

    if (fread(&header, sizeof(t_bmp_header), 1, file) != 1) {
         fprintf(stderr, "Error: Failed to read BMP header from %s.\n", filename);
         fclose(file); return NULL;
    }
    if (fread(&info, sizeof(t_bmp_info), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to read BMP info header from %s.\n", filename);
         fclose(file); return NULL;
    }


    if (header.type != BITMAP_MAGIC) {
        fprintf(stderr, "Error: File %s is not a BMP file (Magic number 0x%X).\n", filename, header.type);
        fclose(file);
        return NULL;
    }
    if (info.size != BMP_INFOHEADER_SIZE) {
         fprintf(stderr, "Warning: BMP info header size is %u, expected %d. May be an unsupported BMP variant.\n", info.size, BMP_INFOHEADER_SIZE);
    }
    if (info.bits != 24) {
        fprintf(stderr, "Error: File %s is not a 24-bit BMP (Bits=%u).\n", filename, info.bits);
        fclose(file);
        return NULL;
    }
     if (info.compression != NO_COMPRESSION) {
        fprintf(stderr, "Error: Compression is not supported (Compression=%u).\n", info.compression);
        fclose(file);
        return NULL;
    }
    if (info.height < 0) {
         fprintf(stderr, "Warning: Image height is negative (top-down BMP). Handling as positive.\n");
         info.height = -info.height;
    }


    t_bmp24 *img = bmp24_allocate(info.width, info.height, info.bits);
    if (!img) {
        fclose(file);
        return NULL;
    }

    img->header = header;
    img->header_info = info;
    img->width = info.width;
    img->height = info.height;
    img->colorDepth = info.bits;


    if (bmp24_readPixelData(img, file) != 0) {
        fprintf(stderr, "Error: Failed to read pixel data from %s.\n", filename);
        fclose(file);
        bmp24_free(img);
        return NULL;
    }

    fclose(file);
    printf("Image '%s' loaded successfully (%dx%d, %d-bit).\n", filename, img->width, img->height, img->colorDepth);
    return img;
}


// [Part 2.4.4 Implementation] Save 24-bit BMP
void bmp24_saveImage(const char *filename, t_bmp24 *img) {
    if (!img || !img->data) {
        fprintf(stderr, "Error: Cannot save NULL or invalid image data.\n");
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s for writing.\n", filename);
        return;
    }

    img->header.type = BITMAP_MAGIC;
    img->header.offset = DEFAULT_OFFSET;
    img->header_info.size = BMP_INFOHEADER_SIZE;
    img->header_info.width = img->width;
    img->header_info.height = img->height;
    img->header_info.planes = 1;
    img->header_info.bits = 24;
    img->header_info.compression = NO_COMPRESSION;
    int row_stride = calculate_row_stride(img->width);
    img->header_info.imagesize = row_stride * img->height;
    img->header.size = img->header.offset + img->header_info.imagesize;

    img->header.reserved1 = 0;
    img->header.reserved2 = 0;
    img->header_info.xresolution = 0;
    img->header_info.yresolution = 0;
    img->header_info.ncolors = 0;
    img->header_info.importantcolors = 0;


    if (fwrite(&img->header, sizeof(t_bmp_header), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to write BMP header to %s.\n", filename);
        fclose(file); return;
    }
     if (fwrite(&img->header_info, sizeof(t_bmp_info), 1, file) != 1) {
        fprintf(stderr, "Error: Failed to write BMP info header to %s.\n", filename);
        fclose(file); return;
    }


    if (bmp24_writePixelData(img, file) != 0) {
        fprintf(stderr, "Error: Failed to write pixel data to %s.\n", filename);
    } else {
         printf("Image saved successfully as %s.\n", filename);
    }

    fclose(file);
}

void bmp24_printInfo(t_bmp24 *img) {
     if (!img) {
        printf("Image Info: No 24-bit image loaded.\n");
        return;
    }
    printf("--- 24-bit Image Info ---\n");
    printf("  Width:       %d\n", img->width);
    printf("  Height:      %d\n", img->height);
    printf("  Color Depth: %d\n", img->colorDepth);
    printf(" Header Fields:\n");
    printf("  File Size:   %u bytes\n", img->header.size);
    printf("  Data Offset: %u\n", img->header.offset);
    printf(" Info Header Fields:\n");
    printf("  Info Size:   %u\n", img->header_info.size);
    printf("  Compression: %u (%s)\n", img->header_info.compression, img->header_info.compression == 0 ? "None" : "Unsupported");
    printf("  Image Size:  %u bytes\n", img->header_info.imagesize);

}


// [Part 2.4.2 Implementation] Read pixel data from file
int bmp24_readPixelData(t_bmp24 *img, FILE *file) {
    if (!img || !img->data || !file) return -1;

    int width = img->width;
    int height = img->height;
    int row_stride = calculate_row_stride(width);
    uint32_t data_offset = img->header.offset;

    unsigned char *row_buffer = (unsigned char *)malloc(row_stride);
    if (!row_buffer) {
        fprintf(stderr, "Error: Failed to allocate buffer for reading rows.\n");
        return -1;
    }

    for (int y = 0; y < height; ++y) {
        uint32_t row_file_offset = data_offset + (height - 1 - y) * row_stride;

        if (fseek(file, row_file_offset, SEEK_SET) != 0) {
             fprintf(stderr, "Error: fseek failed for row %d (file row %d) offset %u\n", y, height - 1- y, row_file_offset);
             free(row_buffer); return -1;
        }
        if (fread(row_buffer, 1, row_stride, file) != row_stride) {
            fprintf(stderr, "Error: Failed to read data for row %d (file row %d).\n", y, height - 1- y);
             if(ferror(file)) perror("fread error"); else if (feof(file)) fprintf(stderr, "fread error: unexpected EOF\n");
            free(row_buffer);
            return -1;
        }

        t_pixel* dest_row = img->data[y];
        for (int x = 0; x < width; ++x) {
            dest_row[x].blue  = row_buffer[x * 3 + 0];
            dest_row[x].green = row_buffer[x * 3 + 1];
            dest_row[x].red   = row_buffer[x * 3 + 2];
        }
    }

    free(row_buffer);
    return 0;
}

// [Part 2.4.2 Implementation] Write pixel data to file
int bmp24_writePixelData(t_bmp24 *img, FILE *file) {
     if (!img || !img->data || !file) return -1;

    int width = img->width;
    int height = img->height;
    int row_stride = calculate_row_stride(width);
    uint32_t data_offset = img->header.offset;

    unsigned char *row_buffer = (unsigned char *)malloc(row_stride);
     if (!row_buffer) {
        fprintf(stderr, "Error: Failed to allocate buffer for writing rows.\n");
        return -1;
    }


    for (int y = 0; y < height; ++y) {
         t_pixel* src_row = img->data[y];

        for (int x = 0; x < width; ++x) {
            row_buffer[x * 3 + 0] = src_row[x].blue;
            row_buffer[x * 3 + 1] = src_row[x].green;
            row_buffer[x * 3 + 2] = src_row[x].red;
        }
        for (int p = width * 3; p < row_stride; ++p) {
            row_buffer[p] = 0;
        }

        uint32_t row_file_offset = data_offset + (height - 1 - y) * row_stride;

        if (fseek(file, row_file_offset, SEEK_SET) != 0) {
             fprintf(stderr, "Error: fseek failed for writing row %d (file row %d) offset %u\n", y, height - 1 - y, row_file_offset);
             free(row_buffer); return -1;
        }
        if (fwrite(row_buffer, 1, row_stride, file) != row_stride) {
            fprintf(stderr, "Error: Failed to write data for row %d (file row %d).\n", y, height - 1 - y);
            if(ferror(file)) perror("fwrite error");
            free(row_buffer);
            return -1;
        }
    }

    free(row_buffer);
    return 0;
}


// [Part 2.5 Implementation] Negative
void bmp24_negative(t_bmp24 *img) {
     if (!img || !img->data) return;
     for (int y = 0; y < img->height; ++y) {
         for (int x = 0; x < img->width; ++x) {
             img->data[y][x].red = 255 - img->data[y][x].red;
             img->data[y][x].green = 255 - img->data[y][x].green;
             img->data[y][x].blue = 255 - img->data[y][x].blue;
         }
     }
     printf("Negative filter applied (24-bit).\n");
}

// [Part 2.5 Implementation] Grayscale (simple average)
void bmp24_grayscale(t_bmp24 *img) {
     if (!img || !img->data) return;
     for (int y = 0; y < img->height; ++y) {
         for (int x = 0; x < img->width; ++x) {
             uint16_t sum = img->data[y][x].red + img->data[y][x].green + img->data[y][x].blue;
             uint8_t avg = (uint8_t)(sum / 3);
             img->data[y][x].red = avg;
             img->data[y][x].green = avg;
             img->data[y][x].blue = avg;
         }
     }
      printf("Grayscale conversion applied (24-bit).\n");
}

// [Part 2.5 Implementation] Brightness
void bmp24_brightness(t_bmp24 *img, int value) {
     if (!img || !img->data) return;
      for (int y = 0; y < img->height; ++y) {
         for (int x = 0; x < img->width; ++x) {
             int r = img->data[y][x].red + value;
             int g = img->data[y][x].green + value;
             int b = img->data[y][x].blue + value;
             // Use clamp_u8 helper
             img->data[y][x].red   = clamp_u8((double)r);
             img->data[y][x].green = clamp_u8((double)g);
             img->data[y][x].blue  = clamp_u8((double)b);
         }
     }
      printf("Brightness adjusted by %d (24-bit).\n", value);
}

// [Part 2.6 Implementation] Convolution Helper: Applies kernel to one pixel
t_pixel bmp24_convolution_helper(t_pixel **original_data, int x, int y, int width, int height, float **kernel, int kernelSize) {
     t_pixel result = {0, 0, 0};
     int n = kernelSize / 2;
     double sumR = 0.0, sumG = 0.0, sumB = 0.0;

     for (int ky = -n; ky <= n; ++ky) {
         for (int kx = -n; kx <= n; ++kx) {
             int pixelY = y + ky;
             int pixelX = x + kx;

             if (pixelX >= 0 && pixelX < width && pixelY >= 0 && pixelY < height) {
                 t_pixel neighbor = original_data[pixelY][pixelX];
                 float k_val = kernel[ky + n][kx + n];

                 sumR += neighbor.red * k_val;
                 sumG += neighbor.green * k_val;
                 sumB += neighbor.blue * k_val;
             }
         }
     }

     result.red = clamp_u8(sumR);
     result.green = clamp_u8(sumG);
     result.blue = clamp_u8(sumB);

     return result;
}

// [Part 2.6 Implementation] Apply Filter Wrapper: Applies kernel to whole image
void bmp24_applyFilter(t_bmp24 *img, float **kernel, int kernelSize) {
     if (!img || !img->data || !kernel || kernelSize <= 0 || kernelSize % 2 == 0) {
        fprintf(stderr, "Error: Invalid arguments for applyFilter (24-bit).\n");
        return;
    }
    int width = img->width;
    int height = img->height;
    int n = kernelSize / 2;

    t_pixel **tempData = bmp24_allocateDataPixels(width, height);
    if (!tempData) {
        fprintf(stderr, "Error: Failed to allocate temp data for filter (24-bit).\n");
        return;
    }
     for (int y = 0; y < height; ++y) {
         memcpy(tempData[y], img->data[y], width * sizeof(t_pixel));
     }

    for (int y = n; y < height - n; ++y) {
        for (int x = n; x < width - n; ++x) {
            img->data[y][x] = bmp24_convolution_helper(tempData, x, y, width, height, kernel, kernelSize);
        }
    }

    bmp24_freeDataPixels(tempData, height);
    printf("Applied %dx%d filter (24-bit).\n", kernelSize, kernelSize);
}


void bmp24_boxBlur(t_bmp24 *img) {
    int size = 3;
    float **kernel = allocate_kernel(size);
    if (!kernel) { fprintf(stderr,"Kernel alloc failed for box blur (24-bit)\n"); return; }
    float val = 1.0f / 9.0f;
    for(int i=0; i<size; ++i) for(int j=0; j<size; ++j) kernel[i][j] = val;
    bmp24_applyFilter(img, kernel, size);
    free_kernel(kernel, size);
}

void bmp24_gaussianBlur(t_bmp24 *img) {
    int size = 3;
    float **kernel = allocate_kernel(size);
    if (!kernel) { fprintf(stderr,"Kernel alloc failed for gaussian blur (24-bit)\n"); return; }
    kernel[0][0] = 1.0f/16.0f; kernel[0][1] = 2.0f/16.0f; kernel[0][2] = 1.0f/16.0f;
    kernel[1][0] = 2.0f/16.0f; kernel[1][1] = 4.0f/16.0f; kernel[1][2] = 2.0f/16.0f;
    kernel[2][0] = 1.0f/16.0f; kernel[2][1] = 2.0f/16.0f; kernel[2][2] = 1.0f/16.0f;
    bmp24_applyFilter(img, kernel, size);
    free_kernel(kernel, size);
}

void bmp24_outline(t_bmp24 *img) {
    int size = 3;
    float **kernel = allocate_kernel(size);
     if (!kernel) { fprintf(stderr,"Kernel alloc failed for outline (24-bit)\n"); return; }
    kernel[0][0] = -1.0f; kernel[0][1] = -1.0f; kernel[0][2] = -1.0f;
    kernel[1][0] = -1.0f; kernel[1][1] =  8.0f; kernel[1][2] = -1.0f;
    kernel[2][0] = -1.0f; kernel[2][1] = -1.0f; kernel[2][2] = -1.0f;
    bmp24_applyFilter(img, kernel, size);
    free_kernel(kernel, size);
}

void bmp24_emboss(t_bmp24 *img) {
    int size = 3;
    float **kernel = allocate_kernel(size);
     if (!kernel) { fprintf(stderr,"Kernel alloc failed for emboss (24-bit)\n"); return; }
    kernel[0][0] = -2.0f; kernel[0][1] = -1.0f; kernel[0][2] =  0.0f;
    kernel[1][0] = -1.0f; kernel[1][1] =  1.0f; kernel[1][2] =  1.0f;
    kernel[2][0] =  0.0f; kernel[2][1] =  1.0f; kernel[2][2] =  2.0f;
    bmp24_applyFilter(img, kernel, size);
    free_kernel(kernel, size);
}

void bmp24_sharpen(t_bmp24 *img) {
     int size = 3;
    float **kernel = allocate_kernel(size);
     if (!kernel) { fprintf(stderr,"Kernel alloc failed for sharpen (24-bit)\n"); return; }
    kernel[0][0] =  0.0f; kernel[0][1] = -1.0f; kernel[0][2] =  0.0f;
    kernel[1][0] = -1.0f; kernel[1][1] =  5.0f; kernel[1][2] = -1.0f;
    kernel[2][0] =  0.0f; kernel[2][1] = -1.0f; kernel[2][2] =  0.0f;
    bmp24_applyFilter(img, kernel, size);
    free_kernel(kernel, size);
}

// [Part 3.4.3 Implementation] Equalize color image using YUV space
void bmp24_equalize(t_bmp24 *img) {
    if (!img || !img->data) return;

    int width = img->width;
    int height = img->height;
    int numPixels = width * height;
    if(numPixels == 0) return;

    uint8_t *y_channel = (uint8_t *)malloc(numPixels * sizeof(uint8_t));
    double *u_channel = (double *)malloc(numPixels * sizeof(double));
    double *v_channel = (double *)malloc(numPixels * sizeof(double));
    unsigned int *y_hist = (unsigned int *)calloc(256, sizeof(unsigned int));
    unsigned int *y_hist_eq = NULL;

    if (!y_channel || !u_channel || !v_channel || !y_hist) {
        fprintf(stderr, "Error: Failed to allocate memory for YUV equalization.\n");
        free(y_channel); free(u_channel); free(v_channel); free(y_hist);
        return;
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = y * width + x;
            t_yuv current_yuv = rgb_to_yuv(img->data[y][x]);


            y_channel[index] = clamp_u8(current_yuv.y);
            u_channel[index] = current_yuv.u;
            v_channel[index] = current_yuv.v;

            y_hist[y_channel[index]]++;
        }
    }

    // Step 2 & 3: Compute normalized CDF for Y channel
    y_hist_eq = bmp8_computeCDF(y_hist, numPixels);
    if (!y_hist_eq) {
        fprintf(stderr, "Error: Failed compute Y channel CDF.\n");
        free(y_channel); free(u_channel); free(v_channel); free(y_hist);
        return;
    }


    // Step 4 & 5: Apply equalization to Y and convert back to RGB
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
             int index = y * width + x;
             double original_u = u_channel[index];
             double original_v = v_channel[index];
             uint8_t original_y_val = y_channel[index];
             double new_y = (double)y_hist_eq[original_y_val];
             t_yuv equalized_yuv = { new_y, original_u, original_v };
             img->data[y][x] = yuv_to_rgb(equalized_yuv);
        }
    }

    printf("Color histogram equalization applied (Y channel).\n");

    free(y_channel);
    free(u_channel);
    free(v_channel);
    free(y_hist);
    free(y_hist_eq);
}