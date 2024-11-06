#include <protogen/utils/utils.h>

namespace protogen {

void writeImageToCanvas(const Magick::Image &img, ICanvas* canvas) {
        const unsigned int width = img.columns();
        const unsigned int height = img.rows();
        const Magick::PixelPacket* pixels = img.getConstPixels(0, 0, width, height);
        for(unsigned int y = 0; y < height; ++y) {
                for(unsigned int x = 0; x < width; ++x) {
                        const Magick::PixelPacket& pixel = pixels[y * width + x];
                        if(pixel.opacity < 255) {
                                canvas->setPixel(x, y, pixel.red, pixel.green, pixel.blue);
                        }
                }
        }
}

std::string read_file_to_str(const std::string& filename) {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
}

}       // namespace