cmake_minimum_required(VERSION 3.16)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file("${PROJECT_SOURCE_DIR}/include/cmake_config.h.in" "${PROJECT_BINARY_DIR}/include/cmake_config.h")

add_executable(${PROJECT_NAME} "src/main.cpp")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME protogen)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")
target_precompile_headers(${PROJECT_NAME} PRIVATE
    "${PROJECT_SOURCE_DIR}/include/common_headers.hpp"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    mouth
    presentation
    server
    state
    installable_headers
)

target_link_libraries(${PROJECT_NAME} PRIVATE graphics_magick)
target_link_libraries(${PROJECT_NAME} PRIVATE httplib)

# installation
install(TARGETS ${PROJECT_NAME}
    DESTINATION bin
)
