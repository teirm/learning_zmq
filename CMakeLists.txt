cmake_minimum_required(VERSION 3.19)

# This is a pure C project
project(learning_zmq VERSION 1.0 LANGUAGES C)

# Project wide setup
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_C_FLAGS "-g -Wall -Wextra")

add_subdirectory(ch_1)
