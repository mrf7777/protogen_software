#ifndef RENDER_SURFACE_H
#define RENDER_SURFACE_H

#include <functional>

#include <led-matrix.h>

#include <protogen/ICanvas.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/IRenderSurface.hpp>

namespace protogen {

class FakeRenderSurface : public IRenderSurface{
public:
    void drawFrame(const std::function<void(ICanvas&)>&) override {}
    Resolution resolution() const override { return Resolution(32, 32); }
};

}   // namespace

#endif