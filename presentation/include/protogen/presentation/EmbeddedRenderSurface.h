#ifndef EMBEDDEDRENDERSURFACE_H
#define EMBEDDEDRENDERSURFACE_H

#include <memory>
#include <stdexcept>

#include <protogen/presentation/render_surface.h>
#include <protogen/Resolution.hpp>

namespace protogen {

class EmbeddedRenderSurface : public IRenderSurface {
public:
    EmbeddedRenderSurface(std::unique_ptr<IRenderSurface>&& source_surface, const Resolution& embedded_resolution);
    void drawFrame(const std::function<void(ICanvas&)>& drawer);
    Resolution resolution() const;

    class EmbeddedResolutionDoesNotFit : std::invalid_argument {
    public:
        EmbeddedResolutionDoesNotFit(const std::string& message) : std::invalid_argument(message) {};
    };
private:
    std::unique_ptr<IRenderSurface> m_sourceSurface;
    Resolution m_embeddedResolution;
};

} // namespace

#endif