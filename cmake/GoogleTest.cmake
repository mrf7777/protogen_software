set(INSTALL_GTEST OFF)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(BUILD_GMOCK TRUE CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)