#include <protogen/apps/HomeDirLocator.h>

#include <cstdlib>
#include <unistd.h>
#include <pwd.h>

namespace protogen {

std::optional<std::string> HomeDirLocator::getUserDataLocation(const std::string& app_id) const {
    // Attempt `HOME` environment variable.
    if(const char * home_path = std::getenv("HOME")) {
        return std::string(home_path) + "/.protogen/" + app_id + "/userdata";
    }

    // Attempt `getpwuid` function.
    if(passwd * pw = getpwuid(getuid())) {
        return std::string(pw->pw_dir) + "/.protogen/" + app_id + "/userdata";
    }

    return {};
}

} // namespace