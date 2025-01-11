#ifndef PROTOGEN_IPROTOGENAPP_H
#define PROTOGEN_IPROTOGENAPP_H

#include <string>
#include <unordered_map>
#include <functional>

#include <protogen/ICanvas.hpp>
#include <protogen/IProportionProvider.hpp>
#include <protogen/Resolution.hpp>
#include <protogen/IAttributeStore.hpp>

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
     * Initialize your app. This will be called after all methods that start
     * with "receive" are called.
     */
    virtual void initialize() = 0;
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
     * These files are expected to be read-only. If you need
     * read-write or write access to files, use the user data directory
     * instead.
     * 
     * When your app is updated, this directory is expected to
     * be cleared and replaced with the new resources.
     */
    virtual void receiveResourcesDirectory(const std::string& resourcesDirectory) = 0;
    /**
     * This is called with the user data directory. This is where
     * your app can save user data. This directory is unique to
     * the user and the app. If you need read-write or write access
     * to files, this is the directory to use. If you need read-only
     * access to files, use the resources directory instead.
     * 
     * When an app is updated, this directory is unchanged.
     */
    virtual void receiveUserDataDirectory(const std::string& userDataDirectory) = 0;

    /**
     * What port your app's web server runs on. If the app does not have a web
     * interface, return -1.
     * 
     * The core software uses a proxy server to forward calls to your web
     * server. Do not prefix your URL paths with any namespacing related to your
     * app because this handled for you. For example, If your `id` method
     * returns "testapp" and your `webPort` method returns 12345, assuming the
     * core software is running on port 80 on host 0.0.0.0, the following
     * request is mapped as follows:
     * GET http://0.0.0.0/apps/testapp/path -> GET http://0.0.0.0:12345/path
     * 
     * Why is it this way? Because HTTP is an interface and lets you decide on
     * how to implement the web server without the core software imposing a C++
     * web interface on you. I recommend cpp-httplib which is found here:
     * https://github.com/yhirose/cpp-httplib.
     */
    virtual int webPort() const {
        return -1;
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

    /**
     * Return attribute store for this app.
     */
    virtual std::shared_ptr<attributes::IAttributeStore> getAttributeStore() = 0;

    // Attribute helpers.
    std::string id() { return getAttributeStore()->getAttribute(attributes::ATTRIBUTE_ID).value_or(""); }
    std::string name() { return getAttributeStore()->getAttribute(attributes::ATTRIBUTE_NAME).value_or(""); }
    std::string description() { return getAttributeStore()->getAttribute(attributes::ATTRIBUTE_DESCRIPTION).value_or(""); }
    std::string thumbnail() { return getAttributeStore()->getAttribute(attributes::ATTRIBUTE_THUMBNAIL).value_or(""); }
    std::string mainPage() { return getAttributeStore()->getAttribute(attributes::ATTRIBUTE_MAIN_PAGE).value_or(""); }
    std::string homePage() { return getAttributeStore()->getAttribute(attributes::ATTRIBUTE_HOME_PAGE).value_or(""); }
};

using CreateAppFunction = IProtogenApp * (*)();
using DestroyAppFunction = void (*)(IProtogenApp *);

}   // namespace

#endif