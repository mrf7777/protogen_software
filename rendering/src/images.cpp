#include <protogen/rendering/images.h>

#include <stdexcept>

namespace protogen {

std::optional<Magick::Image> loadImage(const std::string& filename) {
	Magick::Image image;
	try {
		image.read(filename);
	} catch(Magick::Exception& e) {
		std::cerr << "Problem loading image: " << e.what() << std::endl;
		return {};
	}
	return {image};
}

Spectrum::Spectrum() : Spectrum(0.0, 100.0, 10) {}

Spectrum::Spectrum(double min, double max, unsigned int buckets, double tolerance) {
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
    unsigned int buckets_created = 0;
    while(buckets_created != m_buckets) {
        double bucket_start = m_min + (buckets_created * bucket_distance);
        double bucket_end = bucket_start + bucket_distance + tolerance;
        m_computed_buckets.push_back(std::pair<double, double>(bucket_start, bucket_end));
        buckets_created++;
    }
}

std::size_t Spectrum::bucket(double value) const {
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

ImageSpectrum::ImageSpectrum() {}

ImageSpectrum::ImageSpectrum(const std::string& images_directory) {
    std::vector<std::filesystem::path> files_in_directory;
    // Get all .png files that have only a non-negative integer in the filename.
    for(auto& file : std::filesystem::directory_iterator(images_directory)) {
        const auto file_extention = file.path().extension().string();
        const auto file_stem = file.path().stem().string();
        if(file_extention == ".png") {
            try {
                const int file_stem_int = std::stoi(file_stem);
                if(file_stem_int >= 0) {
                    files_in_directory.push_back(file.path());
                }
            } catch(const std::invalid_argument&) {
                // Do nothing. Ignore images that are not numbered.
            }
        }
    }

    // Sort frames based on the integer in the filename.
    m_spectrum = Spectrum(0.0, 1.0, files_in_directory.size());
    std::sort(files_in_directory.begin(), files_in_directory.end(), [](std::filesystem::path a, std::filesystem::path b){
        const auto a_number = std::stoi(a.stem());
        const auto b_number = std::stoi(b.stem());
        return a_number < b_number;
    });

    for(auto& file : files_in_directory) {
        const auto image = loadImage(file.string());
        if(!image.has_value()) {
            std::cerr << "ImageSpectrum could not load image with filename: " << file.filename().string() << std::endl;
            abort();
        }
        m_images.push_back(image.value());
    }
}

Magick::Image ImageSpectrum::imageForValue(Proportion value) const {
    const auto bucket = m_spectrum.bucket(value);
    return m_images.at(bucket);
}

std::vector<Magick::Image> ImageSpectrum::images() const {
    return m_images;
}

StaticImageDrawer::StaticImageDrawer(const std::string& image_path) {
    auto image_result = loadImage(image_path);
    if(image_result.has_value()) {
        m_image = image_result.value();
    }
}

void StaticImageDrawer::drawToCanvas(ICanvas& canvas) {
    writeImageToCanvas(m_image, &canvas);
}

}   // protogen