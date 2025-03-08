cmake_minimum_required(VERSION 3.16)
project(protogen_project)

include(FetchContent)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTOGEN_RESOURCES_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/protogen/resources")
set(PROTOGEN_APPS_DIR "${CMAKE_INSTALL_PREFIX}/share/protogen/apps")

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
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG 924f214303b860b78350e1e2dfb0521a8724464f
)
FetchContent_Populate(httplib)
add_library(httplib INTERFACE IMPORTED)
set_target_properties(httplib PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${httplib_SOURCE_DIR}"
)

## rpi rgb led matrix library
add_library(rpi_rgb_led_matrix INTERFACE)
target_include_directories(rpi_rgb_led_matrix SYSTEM INTERFACE "$ENV{HOME}/rpi-rgb-led-matrix/include")
target_link_libraries(rpi_rgb_led_matrix INTERFACE "$ENV{HOME}/rpi-rgb-led-matrix/lib/librgbmatrix.a")

## SDL
add_library(SDL INTERFACE)
find_package(SDL2 REQUIRED)
target_include_directories(SDL INTERFACE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SDL INTERFACE ${SDL2_LIBRARIES})

## GoogleTest
set(INSTALL_GTEST OFF)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(apps)
add_subdirectory(extensions)
add_subdirectory(installable_headers)
add_subdirectory(mouth)
add_subdirectory(presentation)
add_subdirectory(rendering)
add_subdirectory(server)
add_subdirectory(state)
add_subdirectory(utils)
add_subdirectory(main)
add_subdirectory(tests)

# Copy resources to build so one can run current build
# with resources without installing resources to system.
add_custom_target(static_files ALL)
add_custom_command(
    TARGET static_files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/resources
    ${CMAKE_BINARY_DIR}/resources
)

install(DIRECTORY resources/
    DESTINATION share/protogen/resources
)