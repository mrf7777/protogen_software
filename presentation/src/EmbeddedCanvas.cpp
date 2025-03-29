#include <protogen/presentation/EmbeddedCanvas.h>

using namespace protogen;

EmbeddedCanvas::EmbeddedCanvas(ICanvas &target_canvas, const Window &window, bool clip_to_window)
    : m_targetCanvas(target_canvas), m_window(window), m_clipToWindow(clip_to_window)
{
}

bool EmbeddedCanvas::clipToWindow() const
{
    return m_clipToWindow;
}

void EmbeddedCanvas::setClipToWindow(bool clip_to_window)
{
    m_clipToWindow = clip_to_window;
}

int EmbeddedCanvas::width() const
{
    return m_window.size.width();
}

int EmbeddedCanvas::height() const
{
    return m_window.size.height();
}

void EmbeddedCanvas::setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
    const Point new_point = translate(x, y);
    if(m_clipToWindow) {
        if(inBounds(new_point.x, new_point.y)) {
            m_targetCanvas.setPixel(new_point.x, new_point.y, red, green, blue);
        }
    } else {
        m_targetCanvas.setPixel(new_point.x, new_point.y, red, green, blue);
    }
    
}

void EmbeddedCanvas::clear()
{
    m_targetCanvas.clear();
}

void EmbeddedCanvas::fill(uint8_t red, uint8_t green, uint8_t blue)
{
    m_targetCanvas.fill(red, green, blue);
}

EmbeddedCanvas::Point EmbeddedCanvas::translate(int x, int y) const
{
    // Translate embedded canvas coordinates to the target canvas coordinates.
    return {x + m_window.top_left_x, y + m_window.top_left_y};
}

bool EmbeddedCanvas::inBounds(int x, int y) const
{
    return (x >= 0 && x < width() && y >= 0 && y < height());
}
