#ifndef RENDER_SURFACE_H
#define RENDER_SURFACE_H

#include <functional>

#include <led-matrix.h>

#include <protogen/ICanvas.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/IRenderSurface.hpp>
#include <protogen/StandardAttributeStore.hpp>

namespace protogen {

class FakeRenderSurface : public IRenderSurface {
public:
    FakeRenderSurface() : m_attributes(new StandardAttributeStore()) {
        m_attributes->setAttribute(attributes::A_ID, "fake_render_surface");
        m_attributes->setAttribute(attributes::A_NAME, "Fake Render Surface");
        m_attributes->setAttribute(attributes::A_DESCRIPTION, "A fake render surface that does nothing.");
    }
    Initialization initialize() override { return Initialization::Success; }
    void drawFrame(const std::function<void(ICanvas&)>&) override {}
    Resolution resolution() const override { return Resolution(512, 512); }
    std::shared_ptr<attributes::IAttributeStore> getAttributeStore() override { return m_attributes; }
private:
    std::shared_ptr<attributes::IAttributeStore> m_attributes;
};

}   // namespace

#endif