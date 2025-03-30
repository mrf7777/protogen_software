#include <protogen/extensions/data_locators/ExtensionDirResourceDataLocator.h>
#include <protogen/StandardAttributes.hpp>

using namespace protogen;

ExtensionDirResourceDataLocator::ExtensionDirResourceDataLocator(const std::string& resources_dir_name)
    : m_resourcesDirName(resources_dir_name)
{
}

std::optional<std::filesystem::path> ExtensionDirResourceDataLocator::getResourceDataDirectory(const ExtensionOriginBundle& bundle) const
{
    if(bundle.extension.get() == nullptr) {
        return {};
    }

    const auto extension_id = bundle.extension->getAttribute(attributes::A_ID);
    if(!extension_id.has_value()) {
        return {};
    }

    return bundle.extension_directory / extension_id.value() / m_resourcesDirName;
}