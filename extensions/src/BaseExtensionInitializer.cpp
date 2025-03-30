#include <protogen/extensions/initializers/BaseExtensionInitializer.h>
#include <protogen/StandardAttributes.hpp>

using namespace protogen;

BaseExtensionInitializer::BaseExtensionInitializer(std::shared_ptr<IExtensionUserDataLocator> userDataLocator, std::shared_ptr<IExtensionResourceDataLocator> resourceDataLocator)
    : m_userDataLocator(userDataLocator), m_resourceDataLocator(resourceDataLocator)
{
}

IExtensionInitializer::Initialization BaseExtensionInitializer::initialize(ExtensionOriginBundle bundle)
{
    const auto user_data_directory = m_userDataLocator->getUserDataDirectory(bundle);
    if(user_data_directory.has_value()) {
        bundle.extension->setAttribute(attributes::A_USER_DATA_DIRECTORY, user_data_directory.value().generic_string());
    }

    const auto resource_data_directory = m_resourceDataLocator->getResourceDataDirectory(bundle);
    if(resource_data_directory.has_value())
    {
        bundle.extension->setAttribute(attributes::A_RESOURCES_DIRECTORY, resource_data_directory.value().generic_string());
    }

    if(bundle.extension->initialize() == IExtension::Initialization::Failure) {
        return IExtensionInitializer::Initialization::Failure;
    }
    
    return IExtensionInitializer::Initialization::Success;
}
