#ifndef PROTOGEN_IPROTOGENAPP_H
#define PROTOGEN_IPROTOGENAPP_H

#include <string>
#include <unordered_map>
#include <functional>

#include <protogen/ICanvas.hpp>
#include <protogen/IProportionProvider.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/Endpoints.hpp>

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
class IProtogenApp {
public:
    virtual ~IProtogenApp() {}

    /**
     * The human-friendly name of your app.
     * This is displayed to the user.
     */
    virtual std::string name() const = 0;
    /**
     * The name of your app as used for technical details
     * such as directory names, url path segments, internal app name,
     * etc. Only use lowercase alphanumeric characters.
     */
    virtual std::string id() const = 0;
    /**
     * A human-readable description of your app.
     * Keep it short and simple. A sentence or two will be fine.
     */
    virtual std::string description() const = 0;

    /**
     * If you need something before you are set to active, this is the
     * method to do checks. Return true if you are ready to be set to active.
     * Return false if something is wrong and write to `errorMessage`
     * what went wrong and what could be done to fix it if possible.
     */
    virtual bool sanityCheck(std::string& errorMessage) const {
        (void)errorMessage;
        return true;
    };
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
     * This is called with the resources directory. This is where
     * your app's resources are located. You can use this to
     * load your resources, or to save data for this app.
     */
    virtual void receiveResourcesDirectory(const std::string& resourcesDirectory) = 0;

    /**
     * Returns all of the web endpoints for this app.
     * 
     * Be aware that the `relativePath` of each Endpoint is relative to the
     * base url: `/apps/{id}` where `{id}` refers to what your
     * `id` method returns.
     * 
     * For example, if your `id` method returns "test",
     * `relativePath` of an Endpoint is "hello", and method is `Get`, then
     * this one Endpoint struct represents the url path "/apps/test/hello"
     * which can be accessed through a http Get request.
     */
    virtual Endpoints serverEndpoints() const = 0;
    /**
     * Relative URL path to the home page of your app.
     * This is what the user will see when they click on your app through
     * the web interface. URL path is relative to "/apps/{id}"
     */
    virtual std::string homePage() const = 0;
    /**
     * Relative URL path to the thumbnail image of your app.
     * This is an image that the user will see as associated with your app.
     * URL path is relative to "/apps/{id}".
     * If returns empty string, a default image will be used.
     */
    virtual std::string thumbnail() const = 0;
    /**
     * Can return empty string if you do not want to host static files.
     * 
     * Returns a relative directory path to your static files that you
     * want the webserver to host. Example: returning "/static_files"
     * will make the webserver host files located at directory
     * "resources/static_files". The "resources" directory is owned by your app,
     * please reference the app directory structure.
     * 
     * Use `staticFilesPath` to specify how access your static files through
     * a URL.
     */
    virtual std::string staticFilesDirectory() const {
        return std::string("");
    };
    /**
     * How to reach the static files through a URL. Example: if this returns
     * "static", you can access your app's static files at URL path: "/apps/{id}/static"
     * where {id} is your app's id.
     * 
     * Assume that there is a file named "image.png" located at "resources/static_files/image.png".
     * If this method returns "static" and `staticFilesDirectory` return "static_files", the image
     * file can be access through the URL path: "/apps/{id}/static/image.png".
     * 
     * Ignored if `staticFilesDirectory` returns empty string.
     */
    virtual std::string staticFilesPath() const {
        return std::string("");
    };

    /**
     * Given a blank canvas, draw the current frame. What you draw will be shown
     * on a video display device, usually a protogen head. The frequency of calls
     * to this method is dictated in part by what your `framerate` method returns.
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
     * The core software at anytime or at any version may put
     * hard limits on the framerate. If your framerate is too
     * high, it may be clamped to a lower rate. Therefore, do not rely
     * on framerate that you provide for other implementation details if possible.
     */
    virtual float framerate() const = 0;
    /**
     * Receive the render surface's resolution.
     * This is called before the `supportedResolutions` method, so
     * feel free to store the device resolution and use it in the   
     * `supportedResolutions` method if you need it.
     */
    virtual void receiveDeviceResolution(const Resolution& device_resolution) = 0;
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
     * This is called with the mouth proportion provider. If you want access to
     * the device which moves the mouth, this is the method to implement.
     */
    virtual void setMouthProportionProvider(std::shared_ptr<IProportionProvider> provider) {
        (void)provider;
    };
};

using CreateAppFunction = IProtogenApp * (*)();
using DestroyAppFunction = void (*)(IProtogenApp *);

}   // namespace

#endif