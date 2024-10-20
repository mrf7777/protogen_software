#ifndef IMAGES_H
#define IMAGES_H

#include <optional>
#include <utility>
#include <algorithm>
#include <filesystem>
#include <cstdlib>

#include <Magick++.h>
#include <magick/image.h>

#include "utils.h"

namespace image {

std::optional<Magick::Image> loadImage(const std::string& filename);

class Spectrum {
public:
	Spectrum();
	Spectrum(double min, double max, unsigned int buckets, double tolerance = 0.1);
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
	 * This class uses a Spectrum to assist in choosing an image
	 * based on some continuous number.
	 */
	ImageSpectrum();
	ImageSpectrum(const std::string& images_directory, double min, double max);
	Magick::Image imageForValue(double value) const;
	std::vector<Magick::Image> images() const;
	Spectrum& spectrum();
private:
	std::vector<Magick::Image> m_images;
	Spectrum m_spectrum;
};

class ImagesDirectoryResource final {
public:
	/**
	 * Expects a directory path with PNG images.
	 * Creates a map between filename and images. Filename
	 * used for the key does not include directory names
	 * or file extensions.
	 */
	ImagesDirectoryResource (const std::string& images_directory);
	std::optional<Magick::Image> getImage(const std::string& key) const;
private:
	std::map<std::string, Magick::Image> m_images;
};

class StaticImageDrawer final {
public:
	StaticImageDrawer(const std::string& image_path);
	void drawToCanvas(rgb_matrix::Canvas& canvas);
private:
	Magick::Image m_image;
};

}

#endif
