cmake_minimum_required(VERSION 3.16)

project(olab0t LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_C_STANDARD 11)

add_subdirectory(src)
