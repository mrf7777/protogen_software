#ifndef PROTOGEN_SENSORSPROVIDER_H
#define PROTOGEN_SENSORSPROVIDER_H

#include <string>
#include <map>
#include <memory>

#include <protogen/extensions/IExtensionCheck.h>
#include <protogen/extensions/IExtensionInitializer.h>
#include <protogen/extensions/IExtensionFinder.h>
#include <protogen/sensors/SensorOriginBundle.h>
#include <protogen/ISensor.hpp>

namespace protogen {

class SensorsProvider {
public:
    SensorsProvider(std::shared_ptr<IExtensionFinder> sensors_finder, std::shared_ptr<IExtensionInitializer> sensor_initializer, std::shared_ptr<IExtensionCheck> sensor_check);
    std::map<std::string, SensorOriginBundle> loadSensors();

private:
    std::shared_ptr<IExtensionFinder> m_sensors_finder;
    std::shared_ptr<IExtensionInitializer> m_sensor_initializer;
    std::shared_ptr<IExtensionCheck> m_sensor_check;
};

} // namespace

#endif