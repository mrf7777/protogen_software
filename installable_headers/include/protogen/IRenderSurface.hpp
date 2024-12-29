#ifndef PROTOGEN_IRENDERSURFACE_HPP
#define PROTOGEN_IRENDERSURFACE_HPP

#include <string>
#include <functional>

#include <protogen/Resolution.hpp>
#include <protogen/ICanvas.hpp>

namespace protogen {

/**
 * Represents an imagery display device.
 * 
 * This is an interface which represents a logical display device.
 * The display device can be physical, digital, or something else.
 * IRenderSurface is responsible only for defining "how" to render
 * imagery, and not the "what"; apps are responsible for the "what".
 * 
 * To install the render surface that you made, observe this directory structure:
 * <install location>/
 * └── shared/
 *     └── protogen/
 *         └── displays/
 *             └── <render surface id as returned by its id method>/
 *                 └── *.so
 * <install location> is the directory where the core protogen software is installed.
 * By default, `cmake --install` on linux installs in /usr/local.
 * When you install the core protogen software, <install location>/shared/protogen
 * is a directory that will exist. When you install your render surface, you are
 * responsible for the following:
 * - Ensure <install location>/shared/protogen/displays/<your render surface id> directory      exists.
 * - Ensure <install location>/shared/protogen/displays/<your render surface id>/<some name>.so exists.
 * 
 * Your .so file is a shared library that is dynamically loaded by the core
 * protogen software. Your .so file must have the following
 * functions implemented:
 * extern "C" IRenderSurface * create_render_surface()
 * extern "C" void destroy_render_surface(IRenderSurface * render_surface)
 * The `create_render_surface` will return a new instance of your class, which implements IRenderSurface and return ownership.
 * The `destroy_render_surface` will take ownership, destroy, and deallocate the class.
 * These both can be implemented as simply as using C++ `new` and `delete` operators. After all,
 * your concrete class can have its own destructor to do cleanup anyway.
 * 
 * The core software will attempt to find a render surface in the
 * directory <install location>/shared/protogen/displays. If the core
 * software finds a render surface that it can use, it will use it.
 */
class IRenderSurface {
public:
    virtual ~IRenderSurface() = default;

    /**
     * The unique identifier of the render surface. This is used to
     * identify the render surface implementation.
     */
    virtual std::string id() const = 0;
    /**
     * The name of the render surface. This is displayed to the user or developer.
     */
    virtual std::string name() const = 0;

    enum class InitializationStatus {
        Success,
        Failure
    };
    /**
     * Initialize render surface. This is where the render surface
     * should be attempted to be initialized. Retuns if initialization
     * was successful or not.
     * 
     * If Failure is returned, the core software will attempt to
     * use a different render surface.
     */
    virtual InitializationStatus initialize() = 0;
    /**
     * Draw a frame to the render surface. You are passed a `drawer`
     * function which defines "what" is to be drawn. Your job as the
     * render surface is to prepare some object which implements
     * ICanvas, which defines the "how" of rendering, and to pass
     * that to the `drawer` function.
     */
    virtual void drawFrame(const std::function<void(ICanvas&)>& drawer) = 0;
    /**
     * Get the resolution of the render surface. Apps may need to
     * know the resolution of the render surface in order to properly
     * render their content. Apps can also use this information to
     * adjust their rendering to fit the resolution of the render
     * surface.
     * 
     * It is not expected to change during the lifetime of the
     * render surface.
     */
    virtual Resolution resolution() const = 0;
};

} // namespace

#endif