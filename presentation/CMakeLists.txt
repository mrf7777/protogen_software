cmake_minimum_required(VERSION 3.16)
project(presentation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTOGEN_SOURCES
    "${PROJECT_SOURCE_DIR}/src/protogen.cpp"
    "${PROJECT_SOURCE_DIR}/src/sdl_render_surface.cpp"
)
add_library(${PROJECT_NAME} ${PROTOGEN_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/installable_headers")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(${PROJECT_NAME} PUBLIC rpi_rgb_led_matrix)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL)
