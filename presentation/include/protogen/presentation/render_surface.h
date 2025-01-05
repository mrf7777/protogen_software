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
    std::string id() const override { return "fake_render_surface"; }
    std::string name() const override { return "Fake Render Surface"; }
    std::string description() const override { return "A fake render surface that does nothing."; }
    InitializationStatus initialize() override { return InitializationStatus::Success; }
    void drawFrame(const std::function<void(ICanvas&)>&) override {}
    Resolution resolution() const override { return Resolution(512, 512); }
};

}   // namespace

#endif