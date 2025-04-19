add_library(rpi_rgb_led_matrix INTERFACE)
target_include_directories(rpi_rgb_led_matrix SYSTEM INTERFACE "$ENV{HOME}/rpi-rgb-led-matrix/include")
target_link_libraries(rpi_rgb_led_matrix INTERFACE "$ENV{HOME}/rpi-rgb-led-matrix/lib/librgbmatrix.a")