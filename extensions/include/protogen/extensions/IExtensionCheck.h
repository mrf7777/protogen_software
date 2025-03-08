#ifndef PROTOGEN_I_EXTENSION_CHECK_H
#define PROTOGEN_I_EXTENSION_CHECK_H

#include <protogen/IExtension.hpp>

#include <string>

namespace protogen {

class IExtensionCheck {
public:
    virtual ~IExtensionCheck() = default;
    virtual bool check(IExtension * extension) = 0;
    virtual std::string error() const = 0;
};

} // namespace

#endif