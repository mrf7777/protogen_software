#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <mutex>

#include <protogen/ICanvas.hpp>

#include <Magick++.h>

namespace protogen {

class IToString {
public:
	~IToString() = default;
	virtual std::string toString() const = 0;
};

void writeImageToCanvas(const Magick::Image &img, ICanvas* canvas);

std::string read_file_to_str(const std::string& filename);

}      // namespace

#endif