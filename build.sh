g++ \
	-std=c++17\
       	-I ~/rpi-rgb-led-matrix/include/\
	-I ./cpp-httplib\
       	-L ~/rpi-rgb-led-matrix/lib\
       	./main.cpp\
       	-l:librgbmatrix.a\
	-o main
