g++ \
	-std=c++20\
       	-I ~/rpi-rgb-led-matrix/include/\
	-I ./cpp-httplib\
       	-L ~/rpi-rgb-led-matrix/lib\
	`GraphicsMagick++-config --cppflags --cxxflags`\
       	./main.cpp\
       	-l:librgbmatrix.a\
	`GraphicsMagick++-config --ldflags --libs`\
	-o main\
	"$@"
