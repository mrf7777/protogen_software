#include <protogen/presentation/EmbeddedCanvas.h>

protogen::EmbeddedCanvas::EmbeddedCanvas(ICanvas &target_canvas, const Resolution &embedded_resolution)
    : m_targetCanvas(target_canvas), m_embeddedResolution(embedded_resolution)
{
}

int protogen::EmbeddedCanvas::width() const
{
    return m_embeddedResolution.width();
}

int protogen::EmbeddedCanvas::height() const
{
    return m_embeddedResolution.height();
}

void protogen::EmbeddedCanvas::setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
    // Translate embedded canvas to the middle of the target canvas.
    const int x_offset = (m_targetCanvas.width() - m_embeddedResolution.width()) / 2;
    const int y_offset = (m_targetCanvas.height() - m_embeddedResolution.height()) / 2;
    const int new_x =  x + x_offset;
    const int new_y = y + y_offset;
    m_targetCanvas.setPixel(new_x, new_y, red, green, blue);
}

void protogen::EmbeddedCanvas::clear()
{
    m_targetCanvas.clear();
}

void protogen::EmbeddedCanvas::fill(uint8_t red, uint8_t green, uint8_t blue)
{
    m_targetCanvas.fill(red, green, blue);
}
