cmake_minimum_required(VERSION 3.16)
project(state)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTOGEN_SOURCES
    "${PROJECT_SOURCE_DIR}/src/minecraft.cpp"
    "${PROJECT_SOURCE_DIR}/src/app_state.cpp"
    "${PROJECT_SOURCE_DIR}/src/minecraft_state.cpp"
    "${PROJECT_SOURCE_DIR}/src/protogen_head_state.cpp"
)
add_library(${PROJECT_NAME} ${PROTOGEN_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} BEFORE PUBLIC "${CMAKE_SOURCE_DIR}/installable_headers")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(${PROJECT_NAME} PUBLIC
    utils
)