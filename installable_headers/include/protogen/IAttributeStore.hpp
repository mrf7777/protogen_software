#ifndef PROTOGEN_IATTRIBUTES_HPP
#define PROTOGEN_IATTRIBUTES_HPP

#include <string>
#include <vector>
#include <optional>

namespace protogen {

/**
 * An interface which represents an object that can have attributes.
 * 
 * Attributes are useful for storing metadata, configuration, and other
 * information that is not part of the core functionality of the object.
 * 
 * Attributes are key-value pairs that can be set, read, and removed.
 * Each attribute has an access level which determines if it can be read,
 * written, or both.
 * 
 * By default, all methods have default implementations that do nothing.
 * You must override these methods to provide attribute functionality.
 */
class IAttributeStore {
public:
    virtual ~IAttributeStore() = default;

    enum class Access {
        Read,
        Write,
        ReadWrite,
    };

    /**
     * Set an attribute with a key and value. If the attribute already exists and it writable, it will be overwritten.
     * Returns true if the attribute was set, false if could not be set.
     * 
     * If a new attribute is created, its access level is determined by the implementation, but it is reasonable to implement it to assign Access of ReadWrite.
     */
    virtual bool setAttribute(const std::string& key, const std::string& value) { (void)key; (void)value; return false; };
    /**
     * Get an attribute value by key. If the attribute does not exist, an empty optional is returned.
     */
    virtual std::optional<std::string> getAttribute(const std::string& key) const { (void)key; return {}; };
    /**
     * Get all attribute keys. If there are no attributes, an empty vector is returned.
     */
    virtual std::vector<std::string> getAttributeKeys() const { return {}; };
    /**
     * Get the access of an attribute by key. If the attribute does not exist, an empty optional is returned.
     */
    virtual std::optional<Access> getAttributeAccess(const std::string& key) const { (void)key; return {}; };
    /**
     * Remove an attribute by key. Returns true if the attribute was removed, false if it could not be removed or does not exist.
     * If the attribute is not writable, it cannot be removed. If its writable, it is up to the implementation to decide if it can be removed.
     */
    virtual bool removeAttribute(const std::string& key) { (void)key; return false; };

    // Helpers

    /**
     * Check if an attribute exists by key.
     */
    virtual bool hasAttribute(const std::string& key) const {
        for(const auto& k : getAttributeKeys()) {
            if(k == key) {
                return true;
            }
        }
        return false;
    };
    /**
     * Check if an attribute is readable by key.
     */
    virtual bool canReadAttribute(const std::string& key) const {
        const auto access = getAttributeAccess(key);
        return access.has_value() && (access.value() == Access::Read || access.value() == Access::ReadWrite);
    };
    /**
     * Check if an attribute is writable by key.
     */
    virtual bool canWriteAttribute(const std::string& key) const {
        const auto access = getAttributeAccess(key);
        return access.has_value() && (access.value() == Access::Write || access.value() == Access::ReadWrite);
    };
    /**
     * Get all readable attribute keys.
     */
    virtual std::vector<std::string> getReadableAttributeKeys() const {
        std::vector<std::string> keys;
        for(const auto& key : getAttributeKeys()) {
            if(canReadAttribute(key)) {
                keys.push_back(key);
            }
        }
        return keys;
    };
    /**
     * Get all writable attribute keys.
     */
    virtual std::vector<std::string> getWritableAttributeKeys() const {
        std::vector<std::string> keys;
        for(const auto& key : getAttributeKeys()) {
            if(canWriteAttribute(key)) {
                keys.push_back(key);
            }
        }
        return keys;
    };
};

} // namespace

#endif