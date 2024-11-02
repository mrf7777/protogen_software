#ifndef PROTOGEN_IPROTOGENAPP_H
#define PROTOGEN_IPROTOGENAPP_H

#include <string>
#include <unordered_map>
#include <functional>

#include <ICanvas.hpp>

#include <httplib.h>

/**
 * An interface which represents an app for a protogen.
 * 
 * An app represents a new feature that the user of the protogen
 * can active and use. Each app defines its own static resources,
 * web server endpoints, internal state, and rendering. All of
 * these allows the app to safely extend the protogen's functionality
 * while not interfering with other protogen apps and core functions.
 * 
 * To install the app that you made, observe this directory structure:
 * /usr/
 * └── local/
 *     └── shared/
 *         └── protogen/
 *             └── apps/
 *                 └── <your app's id as returned by its id method>/
 *                     ├── <some name>.so
 *                     └── resources/
 *                         ├── <some files like .png, .html, etc...>
 *                         ├── image.png
 *                         ├── index.html
 *                         ├── scripts/
 *                         ├── static/
 *                         └── ...
 * When you install the core protogen software, /usr/local/shared/protogen
 * is a directory that will exist. When you install your app, you are
 * responsible for the following:
 * - Ensure /usr/local/shared/protogen/app/<your app id> directory exists.
 * - Ensure /usr/local/shared/protogen/app/<your app id>/resources directory exists.
 * - Ensure /usr/local/shared/protogen/app/<your app id>/<some name>.so exists.
 * 
 * Your .so file is a shared library that is dynamically loaded by the core
 * protogen software. It is this file that contains the concrete class, creation function,
 * and destruction function for the concrete class. Your .so file must have the following
 * functions implemented as well as your concrete class:
 * extern "C" IProtogenApp * create_app()
 * extern "C" void destroy_app(IProtogenApp * app)
 * The `create_app` will return a new instance of your class and return ownership.
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

    enum class HttpMethod {
        Get, Post, Put, Delete, Options, Patch,
    };
    /**
     * A relative path and http method to access some handler.
     * Be aware that the `relativePath` is relative to the
     * base url: `/apps/{id}` where `{id}` refers to what your
     * `id` method returns.
     * 
     * For example, if your `id` method returns "test",
     * `relativePath` is "hello", and method is `Get`, then
     * this endpoint struct represents the url path "/apps/test/hello"
     * which can be accessed through a http Get request.
     */
    struct Endpoint {
        HttpMethod method;
        std::string relativePath;
    };
    /**
     * A function to handle an endpoint being called.
     */
    using EndpointHandler = std::function<void(const httplib::Request&, httplib::Response&)>;
    /**
     * A mapping from endpoints to handlers.
     */
    using Endpoints = std::unordered_map<Endpoint, EndpointHandler>;
    /**
     * Returns the web endpoints for this app.
     */
    virtual Endpoints serverEndpoints() const = 0;
    /**
     * Relative URL path to the home page of your app.
     * This is what the user will see when they click on your app through
     * the web interface. URL path is relative to "/apps/{id}"
     */
    virtual std::string homePage() const = 0;
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
     */
    virtual void render(const ICanvas& canvas) = 0;
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
};

#endif