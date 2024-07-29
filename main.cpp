#include <iostream>
#include <memory>

#include <graphics.h>
#include <canvas.h>
#include <led-matrix.h>
#include <pixel-mapper.h>

#include <httplib.h>

int main() {
	std::cout << "Hello, World!" << std::endl;

	const auto options = rgb_matrix::RGBMatrix::Options();
	const auto runtime_options = rgb_matrix::RuntimeOptions();
	auto matrix = std::unique_ptr<rgb_matrix::RGBMatrix>(rgb_matrix::RGBMatrix::CreateFromOptions(
		options,
		runtime_options
	));

	httplib::Server srv;

	srv.Get("/", [](const httplib::Request & req, httplib::Response & res){
			res.set_content("Hello, World!", "text/plain");
	});

	srv.listen("0.0.0.0", 8080);
}
