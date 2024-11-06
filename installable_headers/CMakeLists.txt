cmake_minimum_required(VERSION 3.16)
project(installable_headers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} INTERFACE)
# Include before system header paths because we do not want to use installed headers.
target_include_directories(${PROJECT_NAME} BEFORE INTERFACE "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} INTERFACE httplib)

install(DIRECTORY include/
    DESTINATION include
)