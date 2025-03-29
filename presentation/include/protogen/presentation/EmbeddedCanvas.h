#ifndef PROTOGEN_EMBEDDEDCANVAS_H
#define PROTOGEN_EMBEDDEDCANVAS_H

#include <protogen/ICanvas.hpp>
#include <protogen/presentation/Window.h>

namespace protogen {

class EmbeddedCanvas : public ICanvas {
public:
    EmbeddedCanvas(ICanvas& target_canvas, const Window& window, bool clip_to_window = true);
    bool clipToWindow() const;
    void setClipToWindow(bool clip_to_window);
    int width() const override;
    int height() const override;
    void setPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue) override;
    void clear() override;
    void fill(uint8_t red, uint8_t green, uint8_t blue) override;
private:
    struct Point {
        int x;
        int y;
    };
    Point translate(int x, int y) const;
    bool inBounds(int x, int y) const;

    ICanvas& m_targetCanvas;
    Window m_window;
    bool m_clipToWindow; // if true, only set pixels within the window bounds.
};

} // namespace

#endif