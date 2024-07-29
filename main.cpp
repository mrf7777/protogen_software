#include <iostream>
#include <memory>

#include <graphics.h>
#include <canvas.h>
#include <led-matrix.h>
#include <pixel-mapper.h>

int main() {
	std::cout << "Hello, World!" << std::endl;

	const auto options = rgb_matrix::RGBMatrix::Options();
	const auto runtime_options = rgb_matrix::RuntimeOptions();
	auto matrix = std::unique_ptr<rgb_matrix::RGBMatrix>(rgb_matrix::RGBMatrix::CreateFromOptions(
		options,
		runtime_options
	));
}
