cmake_minimum_required(VERSION 3.30)
project(Processing_images_in_C C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(Image_mod)

add_executable(Processing_images_in_C
    Image_mod/bmp24.c
    Image_mod/bmp24.h
    Image_mod/bmp8.c
    Image_mod/bmp8.h
    Image_mod/main.c
    Image_mod/utils.c
    Image_mod/utils.h
    functions.c
    functions.h
    main.c)
