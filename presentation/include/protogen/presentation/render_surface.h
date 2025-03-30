#ifndef RENDER_SURFACE_H
#define RENDER_SURFACE_H

#include <functional>

#include <led-matrix.h>

#include <protogen/ICanvas.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/IRenderSurface.hpp>
#include <protogen/StandardAttributeStore.hpp>
#include <protogen/StandardAttributes.hpp>

namespace protogen {

class FakeRenderSurface : public IRenderSurface {
public:
    FakeRenderSurface() : m_attributes(new StandardAttributeStore()) {
        setAttribute(attributes::A_ID, "fake_render_surface");
        setAttribute(attributes::A_NAME, "Fake Render Surface");
        setAttribute(attributes::A_DESCRIPTION, "A fake render surface that does nothing.");
    }
    Initialization initialize() override { return Initialization::Success; }
    void drawFrame(const std::function<void(ICanvas&)>&) override {}
    Resolution resolution() const override { return Resolution(512, 512); }
    
    std::optional<std::string> getAttribute(const std::string& key) const override {
        return m_attributes->getAttribute(key);
    }
    std::vector<std::string> listAttributes() const override {
        return m_attributes->listAttributes();
    }
    SetAttributeResult setAttribute(const std::string& key, const std::string& value) override {
        return m_attributes->setAttribute(key, value);
    }
    RemoveAttributeResult removeAttribute(const std::string& key) override {
        return m_attributes->removeAttribute(key);
    }
    bool hasAttribute(const std::string& key) const override {
        return m_attributes->hasAttribute(key);
    }
private:
    std::shared_ptr<attributes::IAttributeStore> m_attributes;
};

}   // namespace

#endif