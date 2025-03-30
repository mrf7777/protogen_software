#include <protogen/apps/AppsProvider.h>
#include <protogen/StandardAttributes.hpp>

using namespace protogen;

AppsProvider::AppsProvider(std::shared_ptr<IExtensionFinder> apps_finder, std::shared_ptr<IExtensionInitializer> app_initializer, std::shared_ptr<IExtensionCheck> app_check)
    : m_apps_finder(apps_finder), m_app_initializer(app_initializer), m_app_check(app_check)
{
}

std::map<std::string, AppOriginBundle> AppsProvider::loadApps()
{
    auto found_apps = m_apps_finder->find();
    std::map<std::string, AppOriginBundle> apps;
	for(auto& app : found_apps) {
		const auto initialization = m_app_initializer->initialize(app);
		switch(initialization) {
			case IExtensionInitializer::Initialization::Success:
				break;
			case IExtensionInitializer::Initialization::Failure:
				std::cerr << "Failed to initialize app of id `" << app.extension->getAttribute(attributes::A_ID).value_or("<no id>") << "`." << std::endl;
				continue;
				break;
		}
		if(!m_app_check->check(app)) {
			std::cerr
				<< "App of id `"
				<< app.extension->getAttribute(attributes::A_ID).value_or("<no id>")
				<< "` failed to pass checks. Here is the issue: "
				<< m_app_check->error()
				<< std::endl;
			continue;
		}
		apps.insert({
            app.extension->getAttribute(attributes::A_ID).value_or(""),
            AppOriginBundle{
                std::dynamic_pointer_cast<IProtogenApp>(app.extension),
                app.extension_directory
            }
        });
	}
    return apps;
}
