#include <protogen/extensions/initializers/BaseExtensionInitializer.h>

using namespace protogen;

IExtensionInitializer::Initialization BaseExtensionInitializer::initialize(IExtension * extension)
{
    switch(extension->initialize()) {
    case IExtension::Initialization::Success:
        return IExtensionInitializer::Initialization::Success;
    case IExtension::Initialization::Failure:
        return IExtensionInitializer::Initialization::Failure;
    }
}
