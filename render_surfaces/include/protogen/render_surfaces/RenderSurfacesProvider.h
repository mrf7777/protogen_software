#ifndef PROTOGEN_RENDERSURFACESPROVIDER_H
#define PROTOGEN_RENDERSURFACESPROVIDER_H

#include <string>
#include <map>
#include <memory>

#include <protogen/extensions/IExtensionCheck.h>
#include <protogen/extensions/IExtensionInitializer.h>
#include <protogen/extensions/IExtensionFinder.h>
#include <protogen/render_surfaces/RenderSurfaceOriginBundle.h>
#include <protogen/IRenderSurface.hpp>

namespace protogen {

class RenderSurfacesProvider {
public:
    RenderSurfacesProvider(std::shared_ptr<IExtensionFinder> render_surfaces_finder, std::shared_ptr<IExtensionInitializer> render_surface_initializer, std::shared_ptr<IExtensionCheck> render_surface_check);
    std::map<std::string, RenderSurfaceOriginBundle> loadRenderSurfaces();
    
private:
    std::shared_ptr<IExtensionFinder> m_render_surfaces_finder;
    std::shared_ptr<IExtensionInitializer> m_render_surface_initializer;
    std::shared_ptr<IExtensionCheck> m_render_surface_check;
};

} // namespace

#endif