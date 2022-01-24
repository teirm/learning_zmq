cmake_minimum_required(VERSION 3.19)

# This is a pure C project
project(learning_zmq VERSION 1.0 LANGUAGES C)

# Project wide setup
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_C_FLAGS "-g -Wall -Wextra")

# Helper functions
function (add_exec file)
    string(REGEX MATCH "[_A-Za-z0-9]+" name ${file})
    add_executable(${name} ${file})
    set_target_properties(${name}
                      PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/)

    target_link_libraries(${name} PRIVATE zmq)
    target_include_directories(${name}
                               PRIVATE ${PROJECT_SOURCE_DIR}/include)
endfunction()

add_subdirectory(ch_1)
add_subdirectory(ch_2)
