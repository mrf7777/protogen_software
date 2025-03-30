#ifndef PROTOGEN_IWRITABLEATTRIBUTESTORE_HPP
#define PROTOGEN_IWRITABLEATTRIBUTESTORE_HPP

#include <string>

namespace protogen::attributes {

/**
 * Represents an interface for writing attributes to an object.
 */
class IWritableAttributeStore {
public:
    virtual ~IWritableAttributeStore() = default;

    enum class SetAttributeResult {
        Created,
        Updated,
        OtherError,
    };

    enum class RemoveAttributeResult {
        Removed,
        DoesNotExist,
        OtherError,
    };

    /**
     * Creates or updates an attribute with a key and value.
     */
    virtual SetAttributeResult setAttribute(const std::string& key, const std::string& value) = 0;
    
    /**
     * Remove an attribute by key.
     */
    virtual RemoveAttributeResult removeAttribute(const std::string& key) = 0;
};

} // namespace

#endif