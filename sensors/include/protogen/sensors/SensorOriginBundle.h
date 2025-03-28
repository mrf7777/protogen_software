#ifndef PROTOGEN_SENSORORIGINBUNDLE_H
#define PROTOGEN_SENSORORIGINBUNDLE_H

#include <memory>
#include <filesystem>

#include <protogen/ISensor.hpp>

namespace protogen {

    /**
     * A bundle that contains a sensor and the directory where it is located.
     */
    struct SensorOriginBundle {
        std::shared_ptr<sensor::ISensor> sensor;
        std::filesystem::path sensor_directory;
    };
    
} // namespace    

#endif