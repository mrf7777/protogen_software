g++\
	-x c++-header\
	-std=c++20\
       	-I ~/rpi-rgb-led-matrix/include/\
	-I ./cpp-httplib\
       	-L ~/rpi-rgb-led-matrix/lib\
	`GraphicsMagick++-config --cppflags --cxxflags`\
       	-l:librgbmatrix.a\
	`GraphicsMagick++-config --ldflags --libs`\
	-o common_headers.hpp.pch \
	-c common_headers.hpp \
	"$@"
