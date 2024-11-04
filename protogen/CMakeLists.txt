cmake_minimum_required(VERSION 3.16)
project(protogen)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTOGEN_BINARY_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/bin")
set(PROTOGEN_RESOURCES_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/protogen/resources")
set(PROTOGEN_SOURCES
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)
add_executable(${PROJECT_NAME} ${PROTOGEN_SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
configure_file("${PROJECT_SOURCE_DIR}/include/cmake_config.h.in" "${PROJECT_BINARY_DIR}/include/cmake_config.h")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/installable_includes")
# TODO: reintroduce
# target_precompile_headers(${PROJECT_NAME} PRIVATE
#     "${PROJECT_SOURCE_DIR}/include/common_headers.hpp"
# )

target_link_libraries(${PROJECT_NAME} PRIVATE
    mouth
    presentation
    rendering
    server
    state
)

target_link_libraries(${PROJECT_NAME} PRIVATE graphics_magick)
target_link_libraries(${PROJECT_NAME} PRIVATE httplib)

# installation
install(TARGETS protogen
    DESTINATION bin
)
install(DIRECTORY resources/
    DESTINATION share/protogen/resources
)