cmake_minimum_required(VERSION 3.16)
project(tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTOGEN_SOURCES
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
    "${PROJECT_SOURCE_DIR}/src/installable_headers/protogen/StandardAttributeStoreTest.cpp"
    "${PROJECT_SOURCE_DIR}/src/installable_headers/protogen/UniSensorTest.cpp"
)
add_executable(${PROJECT_NAME} ${PROTOGEN_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(${PROJECT_NAME} PUBLIC
    GTest::gtest_main
    GTest::gmock_main
    installable_headers
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})