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

#include <graphics.h>
#include <canvas.h>
#include <led-matrix.h>
#include <pixel-mapper.h>

#include <Magick++.h>

template<typename Data>
class IViewData {
public:
	virtual ~IViewData() = default;
	virtual void viewData(const Data& data) = 0;
};

class IToString {
public:
	~IToString() = default;
	virtual std::string toString() const = 0;
};

void writeImageToCanvas(const Magick::Image &img, rgb_matrix::Canvas* canvas);

class RGBColor : public IToString {
public:
       RGBColor(uint8_t r, uint8_t g, uint8_t b);
       RGBColor(std::string hex);
       uint8_t r() const;
       uint8_t g() const;
       uint8_t b() const;

       virtual std::string toString() const override;
private:
       uint8_t m_r;
       uint8_t m_g;
       uint8_t m_b;
};

std::string read_file_to_str(const std::string& filename);

#endif