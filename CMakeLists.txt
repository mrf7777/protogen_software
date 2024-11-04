cmake_minimum_required(VERSION 3.16)
project(protogen_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
