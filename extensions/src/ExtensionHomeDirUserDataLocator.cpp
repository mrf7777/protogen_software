#include <protogen/extensions/data_locators/ExtensionHomeDirUserDataLocator.h>

#include <cstdlib>
#include <unistd.h>
#include <pwd.h>

using namespace protogen;

ExtensionHomeDirUserDataLocator::ExtensionHomeDirUserDataLocator(const std::string &homeSubDirectory, const std::string &extensionUserDataSubdirectory)
    : m_homeSubDirectory(homeSubDirectory), m_extensionUserDataSubDirectory(extensionUserDataSubdirectory)
{
}

std::optional<std::filesystem::path> ExtensionHomeDirUserDataLocator::getUserDataDirectory(const ExtensionOriginBundle& bundle) const
{
    if(bundle.extension.get() == nullptr) {
        return {};
    }

    const auto extension_id = bundle.extension->getAttributeStore()->getAttribute(attributes::A_ID);
    if(!extension_id.has_value()) {
        return {};
    }

    // Attempt `HOME` environment variable.
    if(const char * home_str = std::getenv("HOME")) {
        const auto home_path = std::filesystem::path(home_str);
        return buildUserDataDirectoryPath(home_path, extension_id.value());
    }

    // Attempt `getpwuid` function.
    if(passwd * pw = getpwuid(getuid())) {
        const auto user_home_path = std::filesystem::path(pw->pw_dir);
        return buildUserDataDirectoryPath(user_home_path, extension_id.value());
    }

    return {};
}

std::filesystem::path protogen::ExtensionHomeDirUserDataLocator::buildUserDataDirectoryPath(const std::filesystem::path &home_path, const std::string &extension_id) const
{
    return home_path / m_homeSubDirectory / extension_id / m_extensionUserDataSubDirectory;
}