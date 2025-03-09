#ifndef PROTOGEN_IEXTENSIONRESOURCEDATALOCATOR_H
#define PROTOGEN_IEXTENSIONRESOURCEDATALOCATOR_H

#include <optional>
#include <filesystem>

#include <protogen/IExtension.hpp>
#include <protogen/extensions/ExtensionOriginBundle.h>

namespace protogen {

class IExtensionResourceDataLocator {
public:
    virtual ~IExtensionResourceDataLocator() = default;
    virtual std::optional<std::filesystem::path> getResourceDataDirectory(const ExtensionOriginBundle& extension) const = 0;
};

} // namespace

#endif