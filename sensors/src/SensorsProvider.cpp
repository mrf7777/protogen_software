#include <protogen/sensors/SensorsProvider.h>

#include <iostream>

using namespace protogen;

protogen::SensorsProvider::SensorsProvider(std::shared_ptr<IExtensionFinder> sensors_finder, std::shared_ptr<IExtensionInitializer> sensor_initializer, std::shared_ptr<IExtensionCheck> sensor_check)
    : m_sensors_finder(sensors_finder), m_sensor_initializer(sensor_initializer), m_sensor_check(sensor_check)
{
}

std::map<std::string, SensorOriginBundle> protogen::SensorsProvider::loadSensors()
{
	auto found_sensors = m_sensors_finder->find();
	std::map<std::string, SensorOriginBundle> sensors;
	for(auto& sensor : found_sensors) {
		const auto initialization = m_sensor_initializer->initialize(sensor);
		switch(initialization) {
			case IExtensionInitializer::Initialization::Success:
				break;
			case IExtensionInitializer::Initialization::Failure:
				std::cerr << "Failed to initialize sensor of id `" << sensor.extension->getAttributeStore()->getAttribute(attributes::A_ID).value_or("<no id>") << "`." << std::endl;
				continue;
				break;
		}
		if(!m_sensor_check->check(sensor)) {
			std::cerr
				<< "Sensor of id `"
				<< sensor.extension->getAttributeStore()->getAttribute(attributes::A_ID).value_or("<no id>")
				<< "` failed to pass checks. Here is the issue: "
				<< m_sensor_check->error()
				<< std::endl;
			continue;
		}
		sensors.insert({
            sensor.extension->getAttributeStore()->getAttribute(attributes::A_ID).value_or(""),
            SensorOriginBundle{
                std::dynamic_pointer_cast<sensor::ISensor>(sensor.extension),
                sensor.extension_directory
            }
        });
	}
    return sensors;
}