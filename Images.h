#ifndef IMAGES_H
#define IMAGES_H

#include <optional>
#include <utility>
#include <algorithm>
#include <filesystem>

#include <Magick++.h>
#include <magick/image.h>

namespace image {

static std::optional<Magick::Image> loadImage(const std::string& filename) {
	Magick::Image image;
	try {
		image.read(filename);
	} catch(Magick::Exception& e) {
		std::cerr << "Problem loading image: " << e.what() << std::endl;
		return {};
	}
	return {image};
}

class Spectrum {
public:
	Spectrum(double min, double max, unsigned int buckets, double tolerance = 0.1) {
		if(min > max) {
			std::swap(min, max);
		}
		if(buckets == 0) {
			buckets = 1;
		}
		m_min = min;
		m_max = max;
		m_buckets = buckets;

		const auto interval_distance = m_max - m_min;
		const auto bucket_distance = interval_distance / m_buckets;
		std::cout << "whole interval distance" << interval_distance << std::endl;
		std::cout << "bucket_distance" << bucket_distance << std::endl;
		int buckets_created = 0;
		while(buckets_created != m_buckets) {
			double bucket_start = m_min + (buckets_created * bucket_distance);
			double bucket_end = bucket_start + bucket_distance + tolerance;
			m_computed_buckets.push_back(std::make_pair(bucket_start, bucket_end));
			buckets_created++;
			std::cout << "start: " << bucket_start << " end: " << bucket_end << std::endl;
		}
		std::cout << "Buckets created: " << buckets_created << std::endl;
	}
	std::size_t bucket(double value) const {
		value = std::clamp(value, m_min, m_max);
		std::size_t index = 0;
		for(const auto& l_bucket : m_computed_buckets) {
			if(l_bucket.first < value && value < l_bucket.second) {
				return index;
			}
			index++;
		}
		return 0;
	}
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
	 * - 3.png
	 * This class uses a Spectrum to assist in choosing an image
	 * based on some continuous number.
	 */
	ImageSpectrum(const std::string& images_directory, Spectrum spectrum) : m_spectrum(spectrum) {
		std::vector<std::filesystem::path> files_in_directory;
		for(const auto& file : std::filesystem::directory_iterator(images_directory)) {
			files_in_directory.push_back(file.path());
		}
		std::sort(files_in_directory.begin(), files_in_directory.end(), [](std::filesystem::path a, std::filesystem::path b){
			const auto a_number = a.filename().string().substr(0, a.filename().string().size()-4);
			const auto b_number = b.filename().string().substr(0, b.filename().string().size()-4);
			return a_number < b_number;
		});

		for(const auto& file : files_in_directory) {
			const auto image = loadImage(file.string());
			if(!image.has_value()) {
				continue;
			}
			m_images.push_back(image.value());
		}
	}
	Magick::Image imageForValue(double value) const {
		const auto bucket = m_spectrum.bucket(value);
		return m_images.at(bucket);
	}
private:
	std::vector<Magick::Image> m_images;
	Spectrum m_spectrum;
};

}

#endif
