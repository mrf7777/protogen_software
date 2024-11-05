cmake_minimum_required(VERSION 3.16)
project(server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTOGEN_SOURCES
    "${PROJECT_SOURCE_DIR}/src/web_server.cpp"
)
add_library(${PROJECT_NAME} ${PROTOGEN_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/installable_headers")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(${PROJECT_NAME} PUBLIC
    utils
    state
)

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
target_compile_options(${PROJECT_NAME} PRIVATE ${GraphicsMagick_CPP_FLAGS})
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${GraphicsMagick_LIBS}
)

# cpp-httplib
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/cpp-httplib
)
