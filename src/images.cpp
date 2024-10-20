#include <images.h>

namespace image {

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
    std::cout << "whole interval distance" << interval_distance << std::endl;
    std::cout << "bucket_distance" << bucket_distance << std::endl;
    unsigned int buckets_created = 0;
    while(buckets_created != m_buckets) {
        double bucket_start = m_min + (buckets_created * bucket_distance);
        double bucket_end = bucket_start + bucket_distance + tolerance;
        m_computed_buckets.push_back(std::pair<double, double>(bucket_start, bucket_end));
        buckets_created++;
        std::cout << "start: " << bucket_start << " end: " << bucket_end << std::endl;
    }
    std::cout << "Buckets created: " << buckets_created << std::endl;
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

ImageSpectrum::ImageSpectrum(const std::string& images_directory, double min, double max) {
    std::vector<std::filesystem::path> files_in_directory;
    for(auto& file : std::filesystem::directory_iterator(images_directory)) {
        std::cout << "ImageSpectrum found file in dir: " << file.path().filename().string() << std::endl;
        files_in_directory.push_back(file.path());
    }
    m_spectrum = Spectrum(min, max, files_in_directory.size());
    std::sort(files_in_directory.begin(), files_in_directory.end(), [](std::filesystem::path a, std::filesystem::path b){
        const auto a_number = a.filename().string().substr(0, a.filename().string().size()-4);
        const auto b_number = b.filename().string().substr(0, b.filename().string().size()-4);
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

Magick::Image ImageSpectrum::imageForValue(double value) const {
    const auto bucket = m_spectrum.bucket(value);
    return m_images.at(bucket);
}

std::vector<Magick::Image> ImageSpectrum::images() const {
    return m_images;
}

Spectrum& ImageSpectrum::spectrum() {
    return m_spectrum;
}

ImagesDirectoryResource::ImagesDirectoryResource (const std::string& images_directory) {
    std::vector<std::filesystem::path> files_in_directory;
    for(const auto& file : std::filesystem::directory_iterator(images_directory)) {
        auto result_image = loadImage(file.path().string());
        if(result_image.has_value()) {
            const auto key = file.path().filename().replace_extension().string();
            m_images.insert({key, result_image.value()});
        }
    }
}
std::optional<Magick::Image> ImagesDirectoryResource::getImage(const std::string& key) const {
    try {
        return {m_images.at(key)};
    } catch(std::out_of_range&) {
        return {};
    }
}

StaticImageDrawer::StaticImageDrawer(const std::string& image_path) {
    auto image_result = image::loadImage(image_path);
    if(image_result.has_value()) {
        m_image = image_result.value();
    }
}

void StaticImageDrawer::drawToCanvas(rgb_matrix::Canvas& canvas) {
    writeImageToCanvas(m_image, &canvas);
}

}