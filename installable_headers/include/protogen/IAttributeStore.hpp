#ifndef PROTOGEN_IATTRIBUTES_HPP
#define PROTOGEN_IATTRIBUTES_HPP

#include <string>
#include <vector>
#include <optional>

// Added extra `attributes` namespace because there are a lot of types and symbols here.
namespace protogen::attributes {

/**
 * An interface which represents an something that can have attributes.
 * 
 * Attributes are useful for storing metadata, configuration, and other
 * information about the implementing object. Attributes can be used to
 * configure the capabilities of a class without changing its C++ API.
 */
class IAttributeStore {
public:
    virtual ~IAttributeStore() = default;

    enum class SetAttributeResult {
        Created,
        Updated,
    };

    enum class RemoveAttributeResult {
        Removed,
        DoesNotExist,
    };

    /**
     * Creates or updates an attribute with a key and value.
     */
    virtual SetAttributeResult setAttribute(const std::string& key, const std::string& value) = 0;
    /**
     * Get an attribute value by key. If the attribute does not exist or is not
     * readable, an empty optional is returned.
     */
    virtual std::optional<std::string> getAttribute(const std::string& key) const = 0;
    /**
     * Get all attribute keys.
     */
    virtual std::vector<std::string> listAttributes() const = 0;
    /**
     * Remove an attribute by key.
     */
    virtual RemoveAttributeResult removeAttribute(const std::string& key) = 0;

    // Helpers

    /**
     * Check if an attribute exists by key.
     */
    virtual bool hasAttribute(const std::string& key) const {
        for(const auto& k : listAttributes()) {
            if(k == key) {
                return true;
            }
        }
        return false;
    };
};

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
// If this attribute exists, the core protogen software will not use the render
// method of the app and the core software will not use a thread to do rendering.
// Instead, the app will receive the render device and the app will be in
// control of rendering. This is good when apps are not compatible with the
// threaded rendering system of the core software.
[[maybe_unused]] static const char * A_CONTROL_RENDER_DEVICE = "control_render_device";

} // namespace

#endif