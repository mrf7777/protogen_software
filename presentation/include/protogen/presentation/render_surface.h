#ifndef RENDER_SURFACE_H
#define RENDER_SURFACE_H

#include <functional>

#include <led-matrix.h>

#include <protogen/ICanvas.hpp>
#include <protogen/Resolution.hpp>

namespace protogen {

/**
 * Represents a physical imagery display device.
 */
class IRenderSurface {
public:
    virtual ~IRenderSurface() = default;
    virtual void drawFrame(const std::function<void(ICanvas&)>& drawer) = 0;
    virtual Resolution resolution() const = 0;
};

class FakeRenderSurface : public IRenderSurface{
public:
    void drawFrame(const std::function<void(ICanvas&)>&) override {}
    Resolution resolution() const override { return Resolution(32, 32); }
};

}   // namespace

#endif