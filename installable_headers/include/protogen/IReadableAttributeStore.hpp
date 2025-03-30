#ifndef PROTOGEN_IREADABLEATTRIBUTESTORE_HPP
#define PROTOGEN_IREADABLEATTRIBUTESTORE_HPP

#include <string>
#include <vector>
#include <optional>

namespace protogen::attributes {

/**
 * Represents an interface for reading attributes from an object.
 */
class IReadableAttributeStore {
public:
    virtual ~IReadableAttributeStore() = default;

    /**
     * Get an attribute value by key. If the attribute does not exist or is not
     * readable, an empty optional is returned.
     */
    virtual std::optional<std::string> getAttribute(const std::string& key) const = 0;

    /**
     * Get all attribute keys.
     */
    virtual std::vector<std::string> listAttributes() const = 0;

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

} // namespace

#endif