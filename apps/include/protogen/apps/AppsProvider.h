#ifndef PROTOGEN_APPSPROVIDER_H
#define PROTOGEN_APPSPROVIDER_H

#include <string>
#include <map>
#include <memory>

#include <protogen/extensions/IExtensionCheck.h>
#include <protogen/extensions/IExtensionInitializer.h>
#include <protogen/extensions/IExtensionFinder.h>
#include <protogen/apps/AppOriginBundle.h>
#include <protogen/IProtogenApp.hpp>

namespace protogen {

class AppsProvider {
public:
    AppsProvider(std::shared_ptr<IExtensionFinder> apps_finder, std::shared_ptr<IExtensionInitializer> app_initializer, std::shared_ptr<IExtensionCheck> app_check);
    std::map<std::string, AppOriginBundle> loadApps();

private:
    std::shared_ptr<IExtensionFinder> m_apps_finder;
    std::shared_ptr<IExtensionInitializer> m_app_initializer;
    std::shared_ptr<IExtensionCheck> m_app_check;
};

} // namespace

#endif