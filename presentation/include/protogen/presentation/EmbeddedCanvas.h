#ifndef EMBEDDEDCANVAS_H
#define EMBEDDEDCANVAS_H

#include <protogen/ICanvas.hpp>
#include <protogen/Resolution.hpp>

namespace protogen {

class EmbeddedCanvas : public ICanvas {
public:
    EmbeddedCanvas(ICanvas& target_canvas, const Resolution& embedded_resolution);
    int width() const override;
    int height() const override;
    void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) override;
    void clear() override;
    void fill(uint8_t red, uint8_t green, uint8_t blue) override;
private:
    ICanvas& m_targetCanvas;
    Resolution m_embeddedResolution;
};

} // namespace

#endif