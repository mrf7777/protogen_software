#ifndef PROTOGEN_IATTRIBUTES_HPP
#define PROTOGEN_IATTRIBUTES_HPP

#include <string>
#include <vector>
#include <optional>

// Added extra `attributes` namespace because there are a lot of types and symbols here.
namespace protogen::attributes {

/**
 * An interface which represents an object that can have attributes.
 * 
 * Attributes are useful for storing metadata, configuration, and other
 * information about the implementing object. Attributes can be used to
 * extend the capabilities of a class without changing its C++ API.
 * 
 * Each attribute has an access level which determines if it can be read,
 * written, or both.
 */
class IAttributeStore {
public:
    virtual ~IAttributeStore() = default;

    enum class Access {
        Read,
        Write,
        ReadWrite,
    };

    enum class SetAttributeResult {
        Created,
        Updated,
        UnsetBecauseNotWritable,
        UnsetBecauseImplementation
    };

    enum class RemoveAttributeResult {
        Removed,
        DoesNotExist,
        KeptBecauseNotWritable,
        KeptBecauseImplementation,
    };

    /**
     * Set an attribute with a key and value.
     * 
     * If the attribute already exists and is writable, it is updated and
     * Updated is returned.
     * 
     * If the attribute does not exist, it is created and Created is returned.
     * The Access of the created attribute is up to the implementation but a
     * sensible default is ReadWrite.
     * 
     * If the attribute exists but is not writable, it is not updated and
     * NotWritable is returned.
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
     * Get the Access of an attribute by key. If the attribute does not exist,
     * an empty optional is returned.
     */
    virtual std::optional<Access> getAttributeAccess(const std::string& key) const = 0;
    /**
     * Remove an attribute by key.
     * 
     * If the key exists and was removed, Removed is returned.
     * 
     * If the key does not exist, DoesNotExist is returned.
     * 
     * If the key exists, but is not writable, KeptBecauseNotWritable is
     * returned.
     * 
     * If the key exists, is writable, but is not removed,
     * KeptBecauseImplementation is returned;
     */
    virtual RemoveAttributeResult removeAttribute(const std::string& key) = 0;
    /**
     * Returns the default access of attributes that are created with
     * `setAttribute`.
     */
    virtual Access getDefaultAccess() const { return Access::ReadWrite; };

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
 * Pre-defined standard attribute keys.
 * 
 * Each pre-defined attribute key has a special use. The use of the word
 * "object" in the following documentation refers to the entity that is
 * associated with the attribute. One such entity is an app.
 */

// ID of the object.
[[maybe_unused]] static const char * ATTRIBUTE_ID = "id";
// Name of the object.
[[maybe_unused]] static const char * ATTRIBUTE_NAME = "name";
// Description of the object.
[[maybe_unused]] static const char * ATTRIBUTE_DESCRIPTION = "description";
// A relative URL path to the thumbnail of the object. This is only the path
// component and not the full URL. For example, if this attribute has value
// "thumbnail.png" and the object id is "test", then a full url to this
// thumbnail from the perspective of this device would be
// http://0.0.0.0/apps/test/thumbnail.png if your object were an app.
[[maybe_unused]] static const char * ATTRIBUTE_THUMBNAIL = "thumbnail";
// A relative URL path to the main page of the app. See comment on
// ATTRIBUTE_THUMBNAIL for details of how this relative URL is used.
// This is where the user is redirected when an app is launched. This can also
// be used to locate a control web interface is which controls some device like
// a sensor or display.
[[maybe_unused]] static const char * ATTRIBUTE_MAIN_PAGE = "main_page";
// A URL to the home page of the app. This is where a user can learn more about
// your object in their free time.
[[maybe_unused]] static const char * ATTRIBUTE_HOME_PAGE = "home_page";
// A URL which locates where to provide funding for the object.
[[maybe_unused]] static const char * ATTRIBUTE_FUNDING = "funding";
// Version of the object. This should follow Semantic Versioning.
// See https://semver.org/.
[[maybe_unused]] static const char * ATTRIBUTE_VERSION = "version";
// Testing phase of the object. It can be one of many values: "alpha", "beta",
// "release_candidate", or "stable".
[[maybe_unused]] static const char * ATTRIBUTE_TESTING_PHASE = "testing_phase";

} // namespace

#endif