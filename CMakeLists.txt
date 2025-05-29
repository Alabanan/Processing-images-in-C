cmake_minimum_required(VERSION 3.30)
project(Image_mod C)

set(CMAKE_C_STANDARD 11)

add_executable(Image_mod main.c
        bmp8.c
        bmp8.h
        bmp24.c
        bmp24.h
        utils.c
        utils.h)
