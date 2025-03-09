#ifndef PROTOGEN_EXTENSIONINSTALLLOCATIONRESOURCEDATALOCATOR_H
#define PROTOGEN_EXTENSIONINSTALLLOCATIONRESOURCEDATALOCATOR_H

#include <protogen/extensions/IExtensionResourceDataLocator.h>

namespace protogen {

class ExtensionDirResourceDataLocator : public IExtensionResourceDataLocator {
public:
    ExtensionDirResourceDataLocator(const std::string& resources_dir_name = "resources");
    std::optional<std::filesystem::path> getResourceDataDirectory(const ExtensionOriginBundle& bundle) const override;
private:
    std::string m_resourcesDirName = "resources";
};

} // namespace

#endif