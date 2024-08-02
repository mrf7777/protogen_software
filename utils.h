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

static void writeImageToCanvas(const Magick::Image &img, rgb_matrix::Canvas* canvas) {
	for(std::size_t y = 0; y < img.rows(); ++y) {
		for(std::size_t x = 0; x < img.columns(); ++x) {
			const Magick::Color& c = img.pixelColor(x, y);
			if(c.alphaQuantum() < 255) {
				canvas->SetPixel(
					x, y,
					ScaleQuantumToChar(c.redQuantum()),
					ScaleQuantumToChar(c.greenQuantum()),
					ScaleQuantumToChar(c.blueQuantum())
				);
			}
		}
	}
}

class RGBColor : public IToString {
public:
       RGBColor(uint8_t r, uint8_t g, uint8_t b) : m_r(r), m_g(g), m_b(b) {}
       RGBColor(std::string hex) {
               if(hex.at(0) == '#') {
                       hex.erase(0, 1);
               }
               while(hex.length() != 6) {
                       hex += "0";
               }
               uint8_t r, g, b;
               sscanf(hex.data(), "%02hhx%02hhx%02hhx", &r, &g, &b);
               m_r = r;
               m_g = g;
               m_b = b;
       }
       uint8_t r() const { return m_r; }
       uint8_t g() const { return m_g; }
       uint8_t b() const { return m_b; }

       virtual std::string toString() const override {
               return "RGBColor{r: " + std::to_string(m_r) + ", g: " + std::to_string(m_g) + ", b: " + std::to_string(m_b) + "}";
       }
private:
       uint8_t m_r;
       uint8_t m_g;
       uint8_t m_b;
};

#endif