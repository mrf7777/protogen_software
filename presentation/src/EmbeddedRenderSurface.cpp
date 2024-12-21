#include <protogen/presentation/EmbeddedRenderSurface.h>
#include <protogen/presentation/EmbeddedCanvas.h>

namespace protogen {

EmbeddedRenderSurface::EmbeddedRenderSurface(std::unique_ptr<IRenderSurface> &&source_surface, const Resolution &embedded_resolution)
    : m_sourceSurface(std::move(source_surface)), m_embeddedResolution(embedded_resolution)
{
    if(!m_embeddedResolution.fitsIn(m_sourceSurface->resolution())) {
        throw EmbeddedResolutionDoesNotFit("embedded resolution `" + m_embeddedResolution.toString() + "` does not fit in device resolution `" + m_sourceSurface->resolution().toString() + "`");
    }
}

void EmbeddedRenderSurface::drawFrame(const std::function<void(ICanvas &)> &drawer)
{
    m_sourceSurface->drawFrame([this, drawer](ICanvas& canvas){
        // canvas is of the source display
        // drawer expects the embedded resolution sized canvas though.
        EmbeddedCanvas embedded_canvas(canvas, m_embeddedResolution);
        drawer(canvas);
    });
}

Resolution EmbeddedRenderSurface::resolution() const
{
    return m_embeddedResolution;
}

} // namespace