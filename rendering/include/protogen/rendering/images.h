#ifndef IMAGES_H
#define IMAGES_H

#include <optional>
#include <utility>
#include <algorithm>
#include <filesystem>
#include <cstdlib>

#include <Magick++.h>
#include <magick/image.h>

#include <protogen/utils/utils.h>
#include <protogen/ICanvas.hpp>
#include <protogen/Proportion.hpp>

namespace protogen {

std::optional<Magick::Image> loadImage(const std::string& filename);

class Spectrum {
public:
	Spectrum();
	Spectrum(double min, double max, unsigned int buckets, double tolerance = 0.000001);
	std::size_t bucket(double value) const;
private:
	double m_min;
	double m_max;
	unsigned int m_buckets;

	std::vector<std::pair<double, double>> m_computed_buckets;
};

class ImageSpectrum final {
public:
	/**
	 * Expects a directory path with PNG images like this:
	 * - 0.png
	 * - 1.png
	 * - 2.png
	 * This class uses a Proportion value (a value in the closed interval [0, 1]) to choose an image.
	 */
	ImageSpectrum();
	ImageSpectrum(const std::string& images_directory);
	Magick::Image imageForValue(Proportion value) const;
	std::vector<Magick::Image> images() const;
private:
	std::vector<Magick::Image> m_images;
	Spectrum m_spectrum;
};

class StaticImageDrawer final {
public:
	StaticImageDrawer(const std::string& image_path);
	void drawToCanvas(ICanvas& canvas);
private:
	Magick::Image m_image;
};

}	// namespace

#endif
