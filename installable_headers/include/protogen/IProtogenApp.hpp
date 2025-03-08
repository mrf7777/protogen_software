#ifndef PROTOGEN_IPROTOGENAPP_H
#define PROTOGEN_IPROTOGENAPP_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

#include <protogen/ICanvas.hpp>
#include <protogen/IProportionProvider.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/IAttributeStore.hpp>
#include <protogen/ISensor.hpp>
#include <protogen/IRenderSurface.hpp>
#include <protogen/IExtension.hpp>

#include <httplib.h>

namespace protogen {

/**
 * An interface which represents an app for a protogen.
 * 
 * An app represents a new feature that the user of the protogen
 * can activate and use. Each app defines its own static resources,
 * web server endpoints, internal state, and rendering logic. All of
 * these allows the app to safely extend the protogen's functionality
 * while not interfering with other protogen apps and core functions.
 * 
 * To install the app that you made, observe this directory structure:
 * <install location>/
 * └── shared/
 *     └── protogen/
 *         └── apps/
 *             └── <app id as returned by its id method>/
 *                 ├── *.so
 *                 └── resources/
 *                     └── <app's files>
 * <install location> is the directory where the core protogen software is installed.
 * By default, `cmake --install` on linux installs in /usr/local.
 * When you install the core protogen software, <install location>/shared/protogen
 * is a directory that will exist. When you install your app, you are
 * responsible for the following:
 * - Ensure <install location>/shared/protogen/app/<your app id>                 directory exists.
 * - Ensure <install location>/shared/protogen/app/<your app id>/resources       directory exists.
 * - Ensure <install location>/shared/protogen/app/<your app id>/<some name>.so  exists.
 * 
 * Your .so file is a shared library that is dynamically loaded by the core
 * protogen software. Your .so file must have the following
 * functions implemented:
 * extern "C" IProtogenApp * create_app()
 * extern "C" void destroy_app(IProtogenApp * app)
 * The `create_app` will return a new instance of your class, which implements IProtogenApp and return ownership.
 * The `destroy_app` will take ownership, destroy, and deallocate the class.
 * These both can be implemented as simply as using C++ `new` and `delete` operators. After all,
 * your concrete class can have its own destructor to do cleanup anyway.
 */
class IProtogenApp : public IExtension {
public:
    virtual ~IProtogenApp() {}
    
    /**
     * When called with true, this app should be ready to go.
     * This is the time to start or resume any background threads or
     * processes.
     * 
     * When called with false, this app is considered not active.
     * Please pause threads to avoid wasting resources and CPU.
     * 
     * By default, you should be inactive.
     */
    virtual void setActive(bool active) = 0;

    /**
     * Given a blank canvas, draw the current frame. What you draw will be shown
     * on a video display device, usually a protogen head. The frequency of calls
     * to this method is dictated in part by what your `framerate` method returns.
     * 
     * This is called in a separate thread from the main thread. If you want to
     * do your own rendering or use an alternative model for rendering, you can
     * set the attribute protogen::attributes::A_CONTROL_RENDER_DEVICE for your
     * app. Setting this attribute will prevent this method from being called.
     * 
     * The resolution of the canvas parameter is determined by an algorithm
     * which selects one of the resolutions that you provide via `supportedResolutions`.
     */
    virtual void render(ICanvas& canvas) const = 0;

    /**
     * The framerate of your app. You may return a different framerate
     * over time, but usually it is just constant. This is used to determine
     * how often your render method is called.
     * 
     * If the app has attribute protogen::attributes::A_CONTROL_RENDER_DEVICE
     * set, this method is not called, ignored, and not used for anything.
     * 
     * The core software at anytime or at any version may put
     * hard limits on the framerate. If your framerate is too
     * high, it may be clamped to a lower rate. Therefore, do not rely
     * on framerate that you provide for other implementation details if possible.
     */
    virtual float framerate() const = 0;

    /**
     * Receive the render surface. This is called before the `render` method
     * and before the `supportedResolutions` method. Feel free to store the
     * render surface and use it in the `render` method. Also feel free to
     * store the resolution of the render surface.
     * 
     * If you set the attribute protogen::attributes::A_CONTROL_RENDER_DEVICE
     * for your app, you are allowed to call `IRenderSurface::drawFrame` in your
     * app. If you have not set this attribute, you are not allowed to call
     * `IRenderSurface::drawFrame` in your app because rendering is being
     * managed by the core software.
     */
    virtual void receiveRenderSurface(std::shared_ptr<IRenderSurface> render_surface) = 0;

    /**
     * The display resolutions that your app supports.
     * These resolutions are compared to the resolutions of the current
     * display device. An algorithm is used to select the "best" resolution
     * from the set that the app provides.
     * 
     * As a tip, the smaller your resolution, the more devices can display your
     * app.
     */
    virtual std::vector<Resolution> supportedResolutions() const = 0;

    /**
     * This is called with all of the registered sensors. If you want access to
     * sensors so that you can read from them, this is the method to implement.
     */
    virtual void receiveSensors(std::vector<std::shared_ptr<protogen::sensor::ISensor>> sensors) {
        (void)sensors;
    };
};

using CreateAppFunction = IProtogenApp * (*)();
using DestroyAppFunction = void (*)(IProtogenApp *);

}   // namespace

#endif