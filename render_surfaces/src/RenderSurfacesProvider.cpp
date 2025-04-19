#include <protogen/render_surfaces/RenderSurfacesProvider.h>
#include <protogen/StandardAttributes.hpp>

#include <iostream>

using namespace protogen;

protogen::RenderSurfacesProvider::RenderSurfacesProvider(std::shared_ptr<IExtensionFinder> render_surfaces_finder, std::shared_ptr<IExtensionInitializer> render_surface_initializer, std::shared_ptr<IExtensionCheck> render_surface_check)
    : m_render_surfaces_finder(render_surfaces_finder), m_render_surface_initializer(render_surface_initializer), m_render_surface_check(render_surface_check)
{
}

std::map<std::string, RenderSurfaceOriginBundle> protogen::RenderSurfacesProvider::loadRenderSurfaces()
{
    auto found_render_surfaces = m_render_surfaces_finder->find();
	std::map<std::string, RenderSurfaceOriginBundle> render_surfaces;
	for(auto& render_surface : found_render_surfaces) {
		const auto initialization = m_render_surface_initializer->initialize(render_surface);
		switch(initialization) {
			case IExtensionInitializer::Initialization::Success:
				break;
			case IExtensionInitializer::Initialization::Failure:
				std::cerr << "Failed to initialize render surface of id `" << render_surface.extension->getAttribute(attributes::A_ID).value_or("<no id>") << "`." << std::endl;
				continue;
				break;
		}
		if(!m_render_surface_check->check(render_surface)) {
			std::cerr
				<< "Render surface of id `"
				<< render_surface.extension->getAttribute(attributes::A_ID).value_or("<no id>")
				<< "` failed to pass checks. Here is the issue: "
				<< m_render_surface_check->error()
				<< std::endl;
			continue;
		}
		render_surfaces.insert({
            render_surface.extension->getAttribute(attributes::A_ID).value_or(""),
            RenderSurfaceOriginBundle{
                std::dynamic_pointer_cast<IRenderSurface>(render_surface.extension),
                render_surface.extension_directory
            }
        });
	}
    return render_surfaces;
}
