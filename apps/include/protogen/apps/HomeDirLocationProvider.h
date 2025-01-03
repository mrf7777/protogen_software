#ifndef HOMEDIRLOCATIONPROVIDER_H
#define HOMEDIRLOCATIONPROVIDER_H

#include <protogen/apps/IUserDataLocationProvider.h>

#include <string>
#include <optional>

namespace protogen {

/**
 * An implementation of IUserDataLocationProvider which provides
 * the user data location for each app as a subdirectory
 * in the user's home directory.
 * 
 * This implementation attempts to read from the HOME environment
 * variable first. If HOME is not set, it will attempt to read
 * using the `getpwuid` function. Therfore, if you run this software
 * using `sudo` command, it will still use the home directory of the
 * user who ran the `sudo` command.
 */
class HomeDirLocationProvider : public IUserDataLocationProvider {
public:
    HomeDirLocationProvider() = default;
    std::optional<std::string> getUserDataLocation(const std::string& app_id) const override;
};

} // namespace

#endif