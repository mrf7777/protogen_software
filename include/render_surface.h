#ifndef RENDER_SURFACE_H
#define RENDER_SURFACE_H

#include <functional>

#include <led-matrix.h>

/**
 * Represents a physical imagery display device.
 */
class IRenderSurface {
public:
    virtual ~IRenderSurface() = default;
    virtual void drawFrame(const std::function<void(rgb_matrix::Canvas&)>& drawer) = 0;
};

class FakeRenderSurface : public IRenderSurface{
public:
    void drawFrame(const std::function<void(rgb_matrix::Canvas&)>&) {}
};

#endif