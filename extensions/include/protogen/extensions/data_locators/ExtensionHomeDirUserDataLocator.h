#ifndef PROTOGEN_EXTNSIONHOMEDIRUSERDATALOCATOR_H
#define PROTOGEN_EXTNSIONHOMEDIRUSERDATALOCATOR_H

#include <filesystem>

#include <protogen/extensions/IExtensionUserDataLocator.h>

namespace protogen {

class ExtensionHomeDirUserDataLocator : public IExtensionUserDataLocator {
public:
    ExtensionHomeDirUserDataLocator(const std::string& homeSubDirectory = ".protogen", const std::string& extensionUserDataSubdirectory = "userdata");
    std::optional<std::filesystem::path> getUserDataDirectory(IExtension * extension) const override;
private:
    std::filesystem::path buildUserDataDirectoryPath(const std::filesystem::path& home_path, const std::string& extension_id) const;

    std::string m_homeSubDirectory;
    std::string m_extensionUserDataSubDirectory;
};

} // namespace

#endif