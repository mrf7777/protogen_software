cmake_minimum_required(VERSION 3.16)
project(protogen_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create targets for external libraries.
## GraphicsMagick
add_library(graphics_magick INTERFACE)
execute_process(
    COMMAND GraphicsMagick++-config --cppflags
    OUTPUT_VARIABLE GraphicsMagick_CPP_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND GraphicsMagick++-config --ldflags --libs
    OUTPUT_VARIABLE GraphicsMagick_LIBS_RAW
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REPLACE "\n" " " GraphicsMagick_LIBS "${GraphicsMagick_LIBS_RAW}")
separate_arguments(GraphicsMagick_LIBS)
target_compile_options(graphics_magick INTERFACE ${GraphicsMagick_CPP_FLAGS})
target_link_libraries(graphics_magick INTERFACE ${GraphicsMagick_LIBS})

## cpp-httplib
add_library(httplib INTERFACE)
target_include_directories(httplib INTERFACE "${CMAKE_SOURCE_DIR}/cpp-httplib")

## rpi rgb led matrix library
add_library(rpi_rgb_led_matrix INTERFACE)
target_include_directories(rpi_rgb_led_matrix SYSTEM INTERFACE "$ENV{HOME}/rpi-rgb-led-matrix/include")
target_link_libraries(rpi_rgb_led_matrix INTERFACE "$ENV{HOME}/rpi-rgb-led-matrix/lib/librgbmatrix.a")

## SDL
add_library(SDL INTERFACE)
find_package(SDL2 REQUIRED)
target_include_directories(SDL INTERFACE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SDL INTERFACE ${SDL2_LIBRARIES})

add_subdirectory(mouth)
add_subdirectory(presentation)
add_subdirectory(protogen)
add_subdirectory(rendering)
add_subdirectory(server)
add_subdirectory(state)
add_subdirectory(utils)

# Copy resources to build so one can run current build
# with resources without installing resources to system.
file(COPY "${PROJECT_SOURCE_DIR}/resources" DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )

install(DIRECTORY resources/
    DESTINATION share/protogen/resources
)