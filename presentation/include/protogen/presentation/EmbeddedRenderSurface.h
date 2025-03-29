#ifndef EMBEDDEDRENDERSURFACE_H
#define EMBEDDEDRENDERSURFACE_H

#include <memory>
#include <stdexcept>

#include <protogen/ICanvas.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/IRenderSurface.hpp>
#include <protogen/StandardAttributeStore.hpp>
#include <protogen/presentation/Window.h>

namespace protogen {

class EmbeddedRenderSurface : public IRenderSurface {
public:
    static std::optional<std::unique_ptr<EmbeddedRenderSurface>> make(std::shared_ptr<IRenderSurface> source_surface, Window window);

    Initialization initialize() override;
    std::shared_ptr<attributes::IAttributeStore> getAttributeStore() override;
    void drawFrame(const std::function<void(ICanvas&)>& drawer) override;
    Resolution resolution() const override;

private:
    class EmbeddedResolutionDoesNotFit : public std::invalid_argument {
    public:
        explicit EmbeddedResolutionDoesNotFit(const std::string& message)
            : std::invalid_argument(message) {}
    };

    EmbeddedRenderSurface(std::shared_ptr<IRenderSurface> source_surface, Window window);

    std::shared_ptr<attributes::IAttributeStore> m_attributes;
    std::shared_ptr<IRenderSurface> m_sourceSurface;
    Window m_window;
};

} // namespace

#endif