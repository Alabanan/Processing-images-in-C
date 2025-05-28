#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp8.h"
#include "bmp24.h"
#include "utils.h"

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void get_filename(char *buffer, int size) {
    printf("Enter file path: ");
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\n")] = 0;
    } else {
        buffer[0] = '\0';
        fprintf(stderr, "Error reading filename.\n");
    }
}


int main() {
    t_bmp8 *img8 = NULL;
    t_bmp24 *img24 = NULL;
    char filename[256];
    int choice = 0;

    while (choice != 99) { // Use 99 for Quit
        printf("\n--- Image Processor ---\n");
        printf("Current Image: ");
        if (img8) printf("8-bit Grayscale Loaded ('%s')\n", filename);
        else if (img24) printf("24-bit Color Loaded ('%s')\n", filename);
        else printf("None Loaded\n");

        printf("Please choose an option:\n");
        printf(" 1. Open 8-bit Grayscale Image (.bmp)\n");
        printf(" 2. Open 24-bit Color Image (.bmp)\n");
        printf(" 3. Save Current Image As...\n");
        printf(" 4. Display Image Information\n");
        printf(" 5. Apply Filter (Basic: Neg/Bright/Thresh/Gray)\n");
        printf(" 6. Apply Filter (Convolution: Blur/Outline/Emboss/Sharpen)\n");
        printf(" 7. Apply Histogram Equalization\n");
        printf("99. Quit\n");
        printf(">>> Your choice: ");

        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Invalid input. Please enter a number.\n");
            clear_input_buffer();
            choice = 0;
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1: // Open 8-bit
                bmp8_free(img8); img8 = NULL;
                bmp24_free(img24); img24 = NULL;
                get_filename(filename, sizeof(filename));
                if (strlen(filename) > 0) {
                    img8 = bmp8_loadImage(filename);
                    if (!img8) filename[0] = '\0';
                }
                break;

            case 2: // Open 24-bit
                bmp8_free(img8); img8 = NULL;
                bmp24_free(img24); img24 = NULL;
                get_filename(filename, sizeof(filename));
                 if (strlen(filename) > 0) {
                    img24 = bmp24_loadImage(filename);
                    if (!img24) filename[0] = '\0';
                }
                break;

            case 3: // Save
                {
                    char save_filename[256];
                    if (img8) {
                        printf("Save 8-bit image as: ");
                        get_filename(save_filename, sizeof(save_filename));
                         if (strlen(save_filename) > 0) bmp8_saveImage(save_filename, img8);
                    } else if (img24) {
                        printf("Save 24-bit image as: ");
                        get_filename(save_filename, sizeof(save_filename));
                        if (strlen(save_filename) > 0) bmp24_saveImage(save_filename, img24);
                    } else {
                        printf("No image loaded to save.\n");
                    }
                }
                break;

            case 4: // Info
                 if (img8) bmp8_printInfo(img8);
                 else if (img24) bmp24_printInfo(img24);
                 else printf("No image loaded.\n");
                 break;

            case 5: // Basic Filters
                 if (img8) {
                     int filter_choice = 0;
                     int value = 0;
                     printf("\n-- 8-bit Basic Filters --\n 1. Negative\n 2. Brightness\n 3. Threshold\n 0. Cancel\n Choice: ");
                     if (scanf("%d", &filter_choice) != 1) { filter_choice = -1; } clear_input_buffer();

                     if (filter_choice == 1) bmp8_negative(img8);
                     else if (filter_choice == 2) { printf("Enter brightness value (-255 to 255): "); if(scanf("%d", &value)==1) { clear_input_buffer(); bmp8_brightness(img8, value); } else {clear_input_buffer();} }
                     else if (filter_choice == 3) { printf("Enter threshold value (0 to 255): "); if(scanf("%d", &value)==1) { clear_input_buffer(); bmp8_threshold(img8, value); } else {clear_input_buffer();} }
                     else if (filter_choice != 0) printf("Invalid filter choice.\n");
                 } else if (img24) {
                     int filter_choice = 0;
                     int value = 0;
                     printf("\n-- 24-bit Basic Filters --\n 1. Negative\n 2. Brightness\n 3. Grayscale\n 0. Cancel\n Choice: ");
                      if (scanf("%d", &filter_choice) != 1) { filter_choice = -1; } clear_input_buffer();

                     if (filter_choice == 1) bmp24_negative(img24);
                     else if (filter_choice == 2) { printf("Enter brightness value (-255 to 255): "); if(scanf("%d", &value)==1) { clear_input_buffer(); bmp24_brightness(img24, value); } else {clear_input_buffer();} }
                     else if (filter_choice == 3) bmp24_grayscale(img24);
                     else if (filter_choice != 0) printf("Invalid filter choice.\n");
                 } else {
                    printf("No image loaded.\n");
                 }
                 break;

            case 6: // Convolution Filters
                 if (img8) {
                      int filter_choice = 0;
                      printf("\n-- 8-bit Convolution Filters --\n 1. Box Blur\n 2. Gaussian Blur\n 3. Outline\n 4. Emboss\n 5. Sharpen\n 0. Cancel\n Choice: ");
                      if (scanf("%d", &filter_choice) != 1) { filter_choice = -1; } clear_input_buffer();

                      if(filter_choice == 1) bmp8_boxBlur(img8);
                      else if(filter_choice == 2) bmp8_gaussianBlur(img8);
                      else if(filter_choice == 3) bmp8_outline(img8);
                      else if(filter_choice == 4) bmp8_emboss(img8);
                      else if(filter_choice == 5) bmp8_sharpen(img8);
                      else if (filter_choice != 0) printf("Invalid filter choice.\n");

                 } else if (img24) {
                      int filter_choice = 0;
                       printf("\n-- 24-bit Convolution Filters --\n 1. Box Blur\n 2. Gaussian Blur\n 3. Outline\n 4. Emboss\n 5. Sharpen\n 0. Cancel\n Choice: ");
                       if (scanf("%d", &filter_choice) != 1) { filter_choice = -1; } clear_input_buffer();

                       if(filter_choice == 1) bmp24_boxBlur(img24);
                       else if(filter_choice == 2) bmp24_gaussianBlur(img24);
                       else if(filter_choice == 3) bmp24_outline(img24);
                       else if(filter_choice == 4) bmp24_emboss(img24);
                       else if(filter_choice == 5) bmp24_sharpen(img24);
                       else if (filter_choice != 0) printf("Invalid filter choice.\n");
                 } else {
                    printf("No image loaded.\n");
                 }
                 break;

             case 7: // Histogram Equalization
                 if (img8) {
                     bmp8_equalize(img8);
                 } else if (img24) {
                     bmp24_equalize(img24);
                 } else {
                     printf("No image loaded.\n");
                 }
                 break;


            case 99: // Quit
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice '%d'. Please try again.\n", choice);
                break;
        }
    }

    bmp8_free(img8);
    bmp24_free(img24);
    return 0;
}