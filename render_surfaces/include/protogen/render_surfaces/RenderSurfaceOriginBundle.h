#ifndef PROTOGEN_RENDERSURFACEORIGINBUNDLE_H
#define PROTOGEN_RENDERSURFACEORIGINBUNDLE_H

#include <memory>
#include <filesystem>

#include <protogen/IRenderSurface.hpp>

namespace protogen {

    /**
     * A bundle that contains a render surface and the directory where it is located.
     */
    struct RenderSurfaceOriginBundle {
        std::shared_ptr<IRenderSurface> render_surface;
        std::filesystem::path render_surface_directory;
    };
    
} // namespace

#endif