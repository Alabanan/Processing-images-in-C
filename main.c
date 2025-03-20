#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

// Function to display the main menu
void displayMainMenu() {
    printf("Please choose an option:\n");
    printf("1. Open an image\n");
    printf("2. Save an image\n");
    printf("3. Apply a filter\n");
    printf("4. Display image information\n");
    printf("5. Quit\n");
    printf(">>> Your choice: ");
}

// Function to display the filter menu
void displayFilterMenu() {
    printf("Please choose a filter:\n");
    printf("1. Negative\n");
    printf("2. Brightness\n");
    printf("3. Black and white\n");
    printf("4. Box Blur\n");
    printf("5. Gaussian blur\n");
    printf("6. Sharpness\n");
    printf("7. Outline\n");
    printf("8. Emboss\n");
    printf("9. Return to the previous menu\n");
    printf(">>> Your choice: ");
}

int main() {
    t_bmp8 * img = NULL; // Pointer to hold the loaded image
    int choice, filterChoice; // Variables to store user choices
    char filePath[256]; // Buffer to store file path
    int brightnessValue, thresholdValue; // Variables for filter parameters

    while (1) { // Infinite loop to keep the menu running
        displayMainMenu(); // Display the main menu
        scanf("%d", &choice); // Read the user's choice

        switch (choice) {
            case 1: // Open an image
                printf("File path: ");
                scanf("%s", filePath); // Read the file path
                img = bmp8_loadImage(filePath); // Load the image
                if (img == NULL) {
                    printf("Error loading image\n");
                } else {
                    printf("Image loaded successfully!\n");
                }
                break;
            case 2: // Save an image
                if (img == NULL) {
                    printf("No image loaded\n");
                } else {
                    printf("File path: ");
                    scanf("%s", filePath); // Read the file path
                    bmp8_saveImage(filePath, img); // Save the image
                    printf("Image saved successfully!\n");
                }
                break;
            case 3: // Apply a filter
                if (img == NULL) {
                    printf("No image loaded\n");
                } else {
                    displayFilterMenu(); // Display the filter menu
                    scanf("%d", &filterChoice); // Read the filter choice
                    switch (filterChoice) {
                        case 1: // Apply negative filter
                            bmp8_negative(img);
                            printf("Filter applied successfully!\n");
                            break;
                        case 2: // Apply brightness filter
                            printf("Brightness value: ");
                            scanf("%d", &brightnessValue); // Read brightness value
                            bmp8_brightness(img, brightnessValue);
                            printf("Filter applied successfully!\n");
                            break;
                        case 3: // Apply threshold filter
                            printf("Threshold value: ");
                            scanf("%d", &thresholdValue); // Read threshold value
                            bmp8_threshold(img, thresholdValue);
                            printf("Filter applied successfully!\n");
                            break;
                        case 4: // Apply box blur filter
                            float boxBlurKernel[3][3] = {
                                {1.0/9, 1.0/9, 1.0/9},
                                {1.0/9, 1.0/9, 1.0/9},
                                {1.0/9, 1.0/9, 1.0/9}
                            };
                            bmp8_applyFilter(img, (float **)boxBlurKernel, 3);
                            printf("Box Blur filter applied successfully!\n");
                            break;
                        case 5: // Apply Gaussian blur filter
                            float gaussianBlurKernel[3][3] = {
                                {1.0/16, 2.0/16, 1.0/16},
                                {2.0/16, 4.0/16, 2.0/16},
                                {1.0/16, 2.0/16, 1.0/16}
                            };
                            bmp8_applyFilter(img, (float **)gaussianBlurKernel, 3);
                            printf("Gaussian Blur filter applied successfully!\n");
                            break;
                        case 6: // Apply sharpness filter
                            float sharpnessKernel[3][3] = {
                                {0, -1, 0},
                                {-1, 5, -1},
                                {0, -1, 0}
                            };
                            bmp8_applyFilter(img, (float **)sharpnessKernel, 3);
                            printf("Sharpness filter applied successfully!\n");
                            break;
                        case 7: // Apply outline filter
                            float outlineKernel[3][3] = {
                                {-1, -1, -1},
                                {-1, 8, -1},
                                {-1, -1, -1}
                            };
                            bmp8_applyFilter(img, (float **)outlineKernel, 3);
                            printf("Outline filter applied successfully!\n");
                            break;
                        case 8: // Apply emboss filter
                            float embossKernel[3][3] = {
                                {-2, -1, 0},
                                {-1, 1, 1},
                                {0, 1, 2}
                            };
                            bmp8_applyFilter(img, (float **)embossKernel, 3);
                            printf("Emboss filter applied successfully!\n");
                            break;
                        case 9: // Return to the previous menu
                            break;
                        default:
                            printf("Invalid choice\n");
                    }
                }
                break;
            case 4: // Display image information
                if (img == NULL) {
                    printf("No image loaded\n");
                } else {
                    bmp8_printInfo(img); // Print image information
                }
                break;
            case 5: // Quit the program
                if (img != NULL) {
                    bmp8_free(img); // Free the loaded image
                }
                return 0; // Exit the program
            default:
                printf("Invalid choice\n");
        }
    }
}