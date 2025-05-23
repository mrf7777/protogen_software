cmake_minimum_required(VERSION 3.16)
project(extensions)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROTOGEN_SOURCES
    "${PROJECT_SOURCE_DIR}/src/BaseExtensionInitializer.cpp"
    "${PROJECT_SOURCE_DIR}/src/DirectoryExtensionFinder.cpp"
    "${PROJECT_SOURCE_DIR}/src/ExtensionDeleter.cpp"
    "${PROJECT_SOURCE_DIR}/src/ExtensionHomeDirUserDataLocator.cpp"
    "${PROJECT_SOURCE_DIR}/src/ExtensionDirResourceDataLocator.cpp"
    "${PROJECT_SOURCE_DIR}/src/RequiredAttributesCheck.cpp"

)
add_library(${PROJECT_NAME} ${PROTOGEN_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(${PROJECT_NAME} PUBLIC
    installable_headers
)