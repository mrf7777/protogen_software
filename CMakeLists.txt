cmake_minimum_required(VERSION 3.16)
project(Protogen)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTOGEN_SOURCES
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/images.cpp"
    "${PROJECT_SOURCE_DIR}/src/minecraft.cpp"
    "${PROJECT_SOURCE_DIR}/src/utils.cpp"
    "${PROJECT_SOURCE_DIR}/src/audio.cpp"
    "${PROJECT_SOURCE_DIR}/src/app_state.cpp"
    "${PROJECT_SOURCE_DIR}/src/minecraft_state.cpp"
    "${PROJECT_SOURCE_DIR}/src/protogen_head_state.cpp"
)
add_executable(protogen ${PROTOGEN_SOURCES})
target_compile_options(protogen PRIVATE -Wall -Wextra)

# GraphicsMagick
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
target_compile_options(protogen PRIVATE ${GraphicsMagick_CPP_FLAGS})
target_link_libraries(protogen PRIVATE
    ${GraphicsMagick_LIBS}
)

# cpp-httplib
target_include_directories(protogen PRIVATE 
    ${PROJECT_SOURCE_DIR}/cpp-httplib
)

# rpi rgb led matrix library
target_include_directories(protogen PRIVATE 
    "$ENV{HOME}/rpi-rgb-led-matrix/include"
)
target_link_libraries(protogen PRIVATE
    "$ENV{HOME}/rpi-rgb-led-matrix/lib/librgbmatrix.a"
)

# protogen project
target_include_directories(protogen PRIVATE 
    "${PROJECT_SOURCE_DIR}/include"
)
target_precompile_headers(protogen PRIVATE
    "${PROJECT_SOURCE_DIR}/include/common_headers.hpp"
)

# install
install(TARGETS protogen
    DESTINATION bin
)
install(DIRECTORY resources/
    DESTINATION share/protogen/resources
)