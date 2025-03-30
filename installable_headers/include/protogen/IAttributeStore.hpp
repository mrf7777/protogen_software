#ifndef PROTOGEN_IATTRIBUTES_HPP
#define PROTOGEN_IATTRIBUTES_HPP

#include <string>
#include <vector>
#include <optional>

#include <protogen/IReadableAttributeStore.hpp>
#include <protogen/IWritableAttributeStore.hpp>

namespace protogen::attributes {

/**
 * An interface which represents an something that can have attributes.
 * 
 * Attributes are useful for storing metadata, configuration, and other
 * information about the implementing object. Attributes can be used to
 * configure the capabilities of a class without changing its C++ API.
 */
class IAttributeStore : public IReadableAttributeStore, public IWritableAttributeStore {
public:
    virtual ~IAttributeStore() = default;
};


} // namespace

#endif