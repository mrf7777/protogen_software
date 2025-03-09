#include <protogen/extensions/data_locators/ExtensionDirResourceDataLocator.h>

using namespace protogen;

ExtensionDirResourceDataLocator::ExtensionDirResourceDataLocator(const std::filesystem::path &extensions_dir, const std::string& resources_dir_name)
    : m_extensionsDir(extensions_dir),
    m_resourcesDirName(resources_dir_name)
{
}

std::optional<std::filesystem::path> ExtensionDirResourceDataLocator::getResourceDataDirectory(IExtension *extension) const
{
    if(extension == nullptr) {
        return {};
    }

    const auto extension_id = extension->getAttributeStore()->getAttribute(attributes::A_ID);
    if(!extension_id.has_value()) {
        return {};
    }

    return m_extensionsDir / extension_id.value() / m_resourcesDirName;
}