#include <protogen/extensions/checks/RequiredAttributesCheck.h>
#include <protogen/StandardAttributes.hpp>
#include <protogen/StandardAttributes.hpp>

#include <string>
#include <vector>

using namespace protogen;

static const std::vector<std::string> REQUIRED_ATTRIBUTES = {
    attributes::A_ID,
    attributes::A_NAME,
    attributes::A_DESCRIPTION,
    attributes::A_AUTHOR,
};

RequiredAttributesCheck::RequiredAttributesCheck()
    : m_error()
{
}

bool RequiredAttributesCheck::check(ExtensionOriginBundle extension)
{
    m_error.clear();

    const auto attributes = extension.extension->getAttributeStore();
    if(attributes.get() == nullptr) {
        m_error = "Extension does not have an attribute store.";
        return false;
    }

    std::vector<std::string> attributes_missing;

    for(const auto& attribute : REQUIRED_ATTRIBUTES) {
        const auto attribute_value = attributes->getAttribute(attribute);
        if(attribute_value.has_value()) {
            if(attribute_value.value().empty()) {
                attributes_missing.push_back(attribute);
            }
        } else {
            attributes_missing.push_back(attribute);
        }
    }

    if(attributes_missing.size() > 0) {
        m_error = makeErrorStringFromMissingAttributes(attributes_missing, attributes->getAttribute(attributes::A_ID));
        return false;
    }

    return true;
}

std::string RequiredAttributesCheck::error() const
{
    return m_error;
}

std::string protogen::RequiredAttributesCheck::makeErrorStringFromMissingAttributes(const std::vector<std::string> &attributes_missing, const std::optional<std::string>& extension_id)
{
    std::string error;
    if(extension_id.has_value()) {
        error = "Extension with id `" + extension_id.value() + "` is missing these required attributes: ";
    } else {
        error = "Extension is missing these required attributes: ";
    }
    std::string missing_attributes_string;
    for(const auto& attribute : attributes_missing) {
        if(missing_attributes_string.empty()) {
            missing_attributes_string = "`" + attribute + "`";
        } else {
            missing_attributes_string += ", `" + attribute + "`";
        }
    }
    error += missing_attributes_string;
    return error;
}
