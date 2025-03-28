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
     * When called with true, your app is considered active and has permission
     * to call rendering methods on the render surface received in a different
     * method.
     * 
     * This is a preferred place to start and pause working threads for the 
     * app based on the active state.
     * 
     * When called with false, this app is considered not active and rendering
     * method calls to the render surface will be ignored.
     * 
     * By default, the app should be inactive.
     */
    virtual void setActive(bool active) = 0;

    /**
     * Receive the render surface. The app is responsible for rendering
     * to the render surface.
     * 
     * If your app is set to inactive, you should call any rendering
     * methods on this render surface; they will be ignored.
     */
    virtual void receiveRenderSurface(std::shared_ptr<IRenderSurface> render_surface) = 0;

    /**
     * This is called with all of the registered sensors.
     */
    virtual void receiveSensors(std::vector<std::shared_ptr<protogen::sensor::ISensor>> sensors) {
        (void)sensors;
    };
};

using CreateAppFunction = IProtogenApp * (*)();
using DestroyAppFunction = void (*)(IProtogenApp *);

}   // namespace

#endif