#ifndef PROTOGEN_BASE_EXTENSION_INITIALIZER_H
#define PROTOGEN_BASE_EXTENSION_INITIALIZER_H

#include <protogen/extensions/IExtensionInitializer.h>
#include <protogen/extensions/ExtensionOriginBundle.h>
#include <protogen/extensions/IExtensionUserDataLocator.h>
#include <protogen/extensions/IExtensionResourceDataLocator.h>

namespace protogen {

class BaseExtensionInitializer : public IExtensionInitializer {
public:
    BaseExtensionInitializer(
        std::shared_ptr<IExtensionUserDataLocator> userDataLocator,
        std::shared_ptr<IExtensionResourceDataLocator> resourceDataLocator);
    Initialization initialize(ExtensionOriginBundle extension) override;
private:
    std::shared_ptr<IExtensionUserDataLocator> m_userDataLocator;
    std::shared_ptr<IExtensionResourceDataLocator> m_resourceDataLocator;
};

} // namespace

#endif