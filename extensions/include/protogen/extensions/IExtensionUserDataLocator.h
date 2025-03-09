#ifndef PROTOGEN_IEXTENSIONUSERDATALOCATOR_H
#define PROTOGEN_IEXTENSIONUSERDATALOCATOR_H

#include <optional>
#include <filesystem>

#include <protogen/IExtension.hpp>
#include <protogen/extensions/ExtensionOriginBundle.h>

namespace protogen {

class IExtensionUserDataLocator {
public:
    virtual ~IExtensionUserDataLocator() = default;
    virtual std::optional<std::filesystem::path> getUserDataDirectory(const ExtensionOriginBundle& extension) const = 0;
};

} // namespace

#endif