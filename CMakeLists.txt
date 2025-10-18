cmake_minimum_required(VERSION 3.10)

project(bfi LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)

add_executable(bfi src/bfi.c)