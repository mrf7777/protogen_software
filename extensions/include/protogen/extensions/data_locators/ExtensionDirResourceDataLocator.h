#ifndef PROTOGEN_EXTENSIONINSTALLLOCATIONRESOURCEDATALOCATOR_H
#define PROTOGEN_EXTENSIONINSTALLLOCATIONRESOURCEDATALOCATOR_H

#include <protogen/extensions/IExtensionResourceDataLocator.h>

namespace protogen {

class ExtensionDirResourceDataLocator : public IExtensionResourceDataLocator {
public:
    ExtensionDirResourceDataLocator(const std::filesystem::path& extensions_dir, const std::string& resources_dir_name = "resources");
    std::optional<std::filesystem::path> getResourceDataDirectory(IExtension * extension) const override;
private:
    std::filesystem::path m_extensionsDir;
    std::string m_resourcesDirName = "resources";
};

} // namespace

#endif