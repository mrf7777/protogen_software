cmake_minimum_required(VERSION 3.16)
project(protogen_project)

include(FetchContent)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTOGEN_RESOURCES_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/protogen/resources")
set(PROTOGEN_APPS_DIR "${CMAKE_INSTALL_PREFIX}/share/protogen/apps")
set(PROTOGEN_SENSORS_DIR "${CMAKE_INSTALL_PREFIX}/share/protogen/sensors")

include(${PROJECT_SOURCE_DIR}/cmake/GraphicsMagick.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CppHttplib.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/RpiRgbLedMatrixLibrary.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/Sdl.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/GoogleTest.cmake)

add_subdirectory(apps)
add_subdirectory(extensions)
add_subdirectory(installable_headers)
add_subdirectory(mouth)
add_subdirectory(presentation)
add_subdirectory(server)
add_subdirectory(state)
add_subdirectory(utils)
add_subdirectory(main)
add_subdirectory(tests)
add_subdirectory(sensors)
add_subdirectory(render_surfaces)

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