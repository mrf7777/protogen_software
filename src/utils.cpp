#include <utils.h>

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

RGBColor::RGBColor(uint8_t r, uint8_t g, uint8_t b) : m_r(r), m_g(g), m_b(b) {}
RGBColor::RGBColor(std::string hex) {
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
uint8_t RGBColor::r() const { return m_r; }
uint8_t RGBColor::g() const { return m_g; }
uint8_t RGBColor::b() const { return m_b; }

std::string RGBColor::toString() const {
        return "RGBColor{r: " + std::to_string(m_r) + ", g: " + std::to_string(m_g) + ", b: " + std::to_string(m_b) + "}";
}

std::string read_file_to_str(const std::string& filename) {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
}

}       // namespace