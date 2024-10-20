#ifndef RENDER_H
#define RENDER_H

#include <vector>
#include <tuple>
#include <cstdint>

namespace render
{

using Color = std::tuple<std::uint8_t, std::uint8_t, std::uint8_t>;

class Render {
public:
    Render(std::size_t width, std::size_t height)
        : m_data(width * height), m_width(width), m_height(height)
    {}

    std::size_t width() const { return m_width; }
    std::size_t height() const { return m_height; }
    Color get(std::size_t x, std::size_t y) const {
        return m_data[x * m_width + y];
    }
    void set(std::size_t x, std::size_t y, Color c) {
        m_data[x * m_width + y] = c;
    }
    
private:
    std::vector<Color> m_data;
    std::size_t m_width;
    std::size_t m_height;
};

}

#endif