#include <protogen/presentation/EmbeddedRenderSurface.h>
#include <protogen/presentation/EmbeddedCanvas.h>
#include <protogen/StandardAttributeStore.hpp>

namespace protogen {

std::optional<std::unique_ptr<EmbeddedRenderSurface>> EmbeddedRenderSurface::make(std::shared_ptr<IRenderSurface> source_surface, Window window)
{
    try
    {
        return std::unique_ptr<EmbeddedRenderSurface>(new EmbeddedRenderSurface(source_surface, window));
    }
    catch(const std::exception& e)
    {
        return {};
    }
    return {};
}

IExtension::Initialization EmbeddedRenderSurface::initialize()
{
    return Initialization::Success;
}

EmbeddedRenderSurface::EmbeddedRenderSurface(std::shared_ptr<IRenderSurface> source_surface, Window window)
    : m_attributes(std::shared_ptr<attributes::IAttributeStore>(new StandardAttributeStore())), m_sourceSurface(source_surface), m_window(window)
{
    const Resolution source_resolution = m_sourceSurface->resolution();
    if (window.top_left_x < 0 || window.top_left_y < 0
        || window.top_left_x + window.size.width() > source_resolution.width()
        || window.top_left_y + window.size.height() > source_resolution.height()) {
        throw EmbeddedResolutionDoesNotFit("embedded window does not fit in the source resolution");
    }
}

void EmbeddedRenderSurface::drawFrame(const std::function<void(ICanvas &)> &drawer)
{
    m_sourceSurface->drawFrame([this, drawer](ICanvas& canvas){
        // canvas is of the source display
        // drawer expects the embedded resolution sized canvas though.
        EmbeddedCanvas embedded_canvas(canvas, m_window);
        drawer(canvas);
    });
}

Resolution EmbeddedRenderSurface::resolution() const
{
    return m_window.size;
}

std::optional<std::string> EmbeddedRenderSurface::getAttribute(const std::string &key) const
{
    return m_attributes->getAttribute(key);
}

std::vector<std::string> EmbeddedRenderSurface::listAttributes() const
{
    return m_attributes->listAttributes();
}

attributes::IWritableAttributeStore::SetAttributeResult EmbeddedRenderSurface::setAttribute(const std::string &key, const std::string &value)
{
    return m_attributes->setAttribute(key, value);
}

attributes::IWritableAttributeStore::RemoveAttributeResult EmbeddedRenderSurface::removeAttribute(const std::string &key)
{
    return m_attributes->removeAttribute(key);
}

bool EmbeddedRenderSurface::hasAttribute(const std::string &key) const
{
    return m_attributes->hasAttribute(key);
}

} // namespace