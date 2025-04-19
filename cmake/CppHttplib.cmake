FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG 924f214303b860b78350e1e2dfb0521a8724464f
)
FetchContent_Populate(httplib)
add_library(httplib INTERFACE IMPORTED)
set_target_properties(httplib PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${httplib_SOURCE_DIR}"
)