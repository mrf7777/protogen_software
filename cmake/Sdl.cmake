add_library(SDL INTERFACE)
find_package(SDL2 REQUIRED)
target_include_directories(SDL INTERFACE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SDL INTERFACE ${SDL2_LIBRARIES})