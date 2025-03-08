#ifndef PROTOGEN_REQUIRED_ATTRIBUTES_CHECK_H
#define PROTOGEN_REQUIRED_ATTRIBUTES_CHECK_H

#include <string>

#include <protogen/IExtension.hpp>
#include <protogen/extensions/IExtensionCheck.h>

namespace protogen {

class RequiredAttributesCheck : public IExtensionCheck {
public:
    RequiredAttributesCheck();
    bool check(IExtension * extension) override;
    std::string error() const override;

private:
    static std::string makeErrorStringFromMissingAttributes(const std::vector<std::string>& attributes_missing, const std::optional<std::string>& extension_id);

    std::string m_error;
};

} // namespace

#endif