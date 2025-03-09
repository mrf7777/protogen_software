#ifndef PROTOGEN_IEXTENSIONUSERDATALOCATOR_H
#define PROTOGEN_IEXTENSIONUSERDATALOCATOR_H

#include <optional>
#include <filesystem>

#include <protogen/IExtension.hpp>

namespace protogen {

class IExtensionUserDataLocator {
public:
    virtual ~IExtensionUserDataLocator() = default;
    virtual std::optional<std::filesystem::path> getUserDataDirectory(IExtension * extension) const = 0;
};

} // namespace

#endif