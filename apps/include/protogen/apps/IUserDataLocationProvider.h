#ifndef IUSERDATALOCATIONPROVIDER_H
#define IUSERDATALOCATIONPROVIDER_H

#include <string>
#include <optional>

namespace protogen {

/**
 * An interface which provides the location of the user data
 * for a given app.
 */
class IUserDataLocationProvider {
public:
    virtual ~IUserDataLocationProvider() = default;

    // Returns the location of the user data for an app as a string.
    // If the location cannot be determined, an empty optional is returned.
    virtual std::optional<std::string> getUserDataLocation(const std::string& app_id) const = 0;
};

} // namespace

#endif // IUSERDATALOCATIONPROVIDER_H