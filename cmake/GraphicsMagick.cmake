add_library(graphics_magick INTERFACE)
execute_process(
    COMMAND GraphicsMagick++-config --cppflags
    OUTPUT_VARIABLE GraphicsMagick_CPP_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND GraphicsMagick++-config --ldflags --libs
    OUTPUT_VARIABLE GraphicsMagick_LIBS_RAW
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REPLACE "\n" " " GraphicsMagick_LIBS "${GraphicsMagick_LIBS_RAW}")
separate_arguments(GraphicsMagick_LIBS)
target_compile_options(graphics_magick INTERFACE ${GraphicsMagick_CPP_FLAGS})
target_link_libraries(graphics_magick INTERFACE ${GraphicsMagick_LIBS})