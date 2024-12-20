#ifndef RESOLUTION_H
#define RESOLUTION_H

namespace protogen {

class Resolution {
public:
    Resolution(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}
    unsigned int width() const { return m_width; }
    unsigned int height() const { return m_height; }
    bool fitsIn(const Resolution& r) const {
        return width() <= r.width() && height() <= r.height();
    }
private:
    unsigned int m_width;
    unsigned int m_height;
};

} // namespace

#endif