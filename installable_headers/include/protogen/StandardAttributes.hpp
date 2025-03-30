#ifndef PROTOGEN_STANDARDATTRIBUTES_HPP
#define PROTOGEN_STANDARDATTRIBUTES_HPP

namespace protogen::attributes {

/**
 * Pre-defined standard attribute keys. Each pre-defined attribute key has a
 * specific use.
 */

// ID of the object.
[[maybe_unused]] static const char * A_ID = "id";

// Name of the object.
[[maybe_unused]] static const char * A_NAME = "name";

// Description of the object.
[[maybe_unused]] static const char * A_DESCRIPTION = "description";

// The author of the object.
[[maybe_unused]] static const char * A_AUTHOR = "author";

// What port your object's web server runs on.
// The core software uses a proxy server to forward calls to your web
// server. Do not prefix your URL paths with any namespacing related to your
// object because this handled for you. For example, If your `id` method
// returns "testapp" and your `webPort` method returns 12345, assuming the
// core software is running on port 80 on host 0.0.0.0, the following
// request is mapped as follows:
// GET http://0.0.0.0/apps/testapp/path -> GET http://0.0.0.0:12345/path
// Why is it this way? Because HTTP is an interface and lets you decide on
// how to implement the web server without the core software imposing a C++
// web interface on you. I recommend cpp-httplib which is found here:
// https://github.com/yhirose/cpp-httplib.
[[maybe_unused]] static const char * A_WEB_PORT = "web_port";

// A relative URL path to the thumbnail of the object. This is only the path
// component and not the full URL. For example, if this attribute has value
// "thumbnail.png" and the object id is "test", then a full url to this
// thumbnail from the perspective of this device would be
// http://0.0.0.0/apps/test/thumbnail.png if your object were an app.
[[maybe_unused]] static const char * A_THUMBNAIL = "thumbnail";

// A relative URL path to the main page of the app. See comment on
// A_THUMBNAIL for details of how this relative URL is used.
// This is where the user is redirected when an app is launched. This can also
// be used to locate a control web interface is which controls some device like
// a sensor or display.
[[maybe_unused]] static const char * A_MAIN_PAGE = "main_page";

// A URL to the home page of the app. This is where a user can learn more about
// your object in their free time.
[[maybe_unused]] static const char * A_HOME_PAGE = "home_page";

// A URL which locates where to provide funding for the object.
[[maybe_unused]] static const char * A_FUNDING = "funding";

// Version of the object. This should follow Semantic Versioning.
// See https://semver.org/.
[[maybe_unused]] static const char * A_VERSION = "version";

// This is where your app's resources are located.
// This attribute is set by the core protogen software when the app is loaded.
// You can use this to load your resources, or to save data for this app.
// These files are expected to be read-only. If you need read-write or write 
// access to files, use the user data directory instead.
[[maybe_unused]] static const char * A_RESOURCES_DIRECTORY = "resources_directory";

// This is where your app can save user data.
// This attribute is set by the core protogen software when the app is loaded.
// This directory is unique to the user and the app. If you need read-write or
// write access to files, this is the directory to use. If you need read-only
// access to files, use the resources directory instead.
[[maybe_unused]] static const char * A_USER_DATA_DIRECTORY = "user_data_directory";
    
} // namespace

#endif