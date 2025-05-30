#ifndef PROTOGEN_ISENSOR_HPP
#define PROTOGEN_ISENSOR_HPP

#include <protogen/IExtension.hpp>

#include <string>
#include <optional>
#include <variant>
#include <vector>
#include <array>
#include <chrono>

// Added extra `sensor` namespace because there are a lot of types and symbols here.
namespace protogen::sensor {

using ChannelReal = double;
using ChannelString = std::string;
using ChannelRealArray = std::vector<ChannelReal>;
using ChannelStringArray = std::vector<ChannelString>;
using ChannelValue = std::variant<
    ChannelReal,
    ChannelString,
    ChannelRealArray,
    ChannelStringArray
>;

/**
 * An interface which represents a sensor that can be attached to a protogen.
 * 
 * A sensor is a device that can read data from the environment and provide
 * that data to the protogen. This data is queried from the apps on the protogen.
 * 
 * A sensor can have multiple channels. Each channel is intended to measure
 * a specific property of some environment. There are standard channels that
 * are pre-defined for common properties such as temperature, sound, location,
 * etc.
 * 
 * To install the sensor that you made, observe this directory structure:
 * <install location>/
 * └── shared/
 *     └── protogen/
 *         └── sensors/
 *             └── <sensor id as returned by its id attribute>/
 *                 └── *.so
 *                 └── resources/
 *                     └── <sensor's files>
 * <install location> is the directory where the core protogen software is installed.
 * By default, `cmake --install` on linux installs in /usr/local.
 * When you install the core protogen software, <install location>/shared/protogen
 * is a directory that will exist. When you install your sensor, you are
 * responsible for the following:
 * - Ensure <install location>/shared/protogen/sensors/<your sensor id>                 directory exists.
 * - Ensure <install location>/shared/protogen/sensors/<your sensor id>/<some name>.so  exists.
 * 
 * Your .so file is a shared library that is dynamically loaded by the core
 * protogen software. Your .so file must have the following
 * functions implemented:
 * extern "C" ISensor * create_sensor()
 * extern "C" void destroy_sensor(ISensor * sensor)
 * The `create_sensor` will return a new instance of your class, which implements ISensor and return ownership.
 * The `destroy_sensor` will take ownership, destroy, and deallocate the class.
 * These both can be implemented as simply as using C++ `new` and `delete` operators. After all,
 * your concrete class can have its own destructor to do cleanup anyway.
 * 
 * Custom channels can be specified by the sensor. The sensor is responsible
 * for documenting the properties of the custom channels it provides. Please
 * make sure there is not already a standard channel that fits your needs or
 * another community which has defined their own standard channels.
 * By using standard channels or community-invented channels, you can ensure
 * that your sensor is compatible with a wide variety of apps.
 * 
 * While each sensor has an id, each channel has an id as well. Sensor ids are
 * required to be unique across all sensors, but two different sensors can have
 * channels with the same id. This is because two sensors may be installed that
 * measure the same property in different ways. For example, one sensor may
 * measure have a channel of id `std:in.temperature` and another sensor may
 * have the same channel, but measure the temperature in a different way.
 * This is a feature, because the core software may choose a sensor's
 * implementation of a channel over another based on a priority configuration.
 * 
 * Each channel id is formatted as: `<origin>:<property>`.
 * - `<origin>`:
 *   - A string which identifies an authority or namespace.
 *   - Can be separated by periods (.)
 *   - This is either `std` for standard channels, or a unique value for groups of custom channels, preferably a domain name or community name.
 * - `<property>`:
 *   - The specific property that the channel measures.
 *   - This should be a short, descriptive string that indicates what the channel measures.
 *   - The property may have periods (.)
 * Some examples:
 * - `std:in.temperature` names the standard channel for the internal temperature of the protogen. `in.temperature` is the property that is measured.
 * - `std:out.temperature` names the standard channel for the external temperature of the protogen. `out.temperature` is the property that is measured.
 * - `ai:digit` could name a custom channel that uses AI to detect digits and returns a string for the digit.
 * - `mycompany:outside.weather` could name a custom channel that measures the chance of rain by device made by `mycompany`.
 * - `mycompany:house.attic.temperature` could name a custom channel that measures the temperature of the attic in a house by device made by `mycompany`.
 */
class ISensor : public IExtension {
public:
    enum class ReadError {
        ChannelNotFound,
        SensorNotReady,
        HardwareFailure,
        UnknownError,
    };

    struct ReadResult {
        std::chrono::system_clock::time_point timestamp;
        std::optional<ChannelValue> value;
        std::optional<ReadError> error;
    };

    /**
     * Information about a channel that the sensor provides.
     * 
     * A channel is a property that the sensor can read from. Each channel has a
     * unique id and a description. The description should detail what the
     * channel measures, how the value is interpreted, and what units the value
     * is in.
     * 
     * Please prefer using standard channels when possible.
     */
    struct ChannelInfo {
        std::string id;
        std::string description;
    };

    virtual ~ISensor() = default;

    /**
     * The channels that the sensor provides. This is a list of the channels that
     * the sensor can read from.
     */
    virtual std::vector<ChannelInfo> channels() const = 0;

    /**
     * A convenience method to get the information about a specific channel.
     */
    virtual std::optional<ChannelInfo> channelInfo(const std::string& channel_id) const {
        for(const auto& channel : channels()) {
            if(channel.id == channel_id) {
                return channel;
            }
        }
        return {};
    };

    /**
     * Read a value from a channel. Use the `ISensor::channels` method to
     * get what channels are available for the sensor.
     * 
     * If the channel is not found or an error occurs, the `error` field in the `ReadResult` will be set accordingly.
     */
    virtual ReadResult read(const std::string& channel_id) = 0;
};

/*
 * Pre-defined standard sensor channels.
 * 
 * These common channels are intended to be used by apps to query data from
 * sensors.
 * 
 * When writing a sensor, it is recommended to use these standard channels
 * when possible. This will make your sensor compatible with a wide variety
 * of apps, and prevent app developers from having to make custom code to
 * support your specific sensor.
 */
const std::array<ISensor::ChannelInfo, 11> STANDARD_CHANNELS = {{
    { 
        "std:in.temperature", 
        "Internal temperature measured in Celsius." 
    },
    { 
        "std:out.temperature", 
        "External temperature measured in Celsius." 
    },
    { 
        "std:out.ambient_light", 
        "Ambient light intensity measured in lux." 
    },
    { 
        "std:out.sound_level", 
        "External sound intensity level measured in decibels (dB)." 
    },
    { 
        "std:in.sound_level", 
        "Internal sound intensity level measured in decibels (dB)." 
    },
    { 
        "std:out.proximity", 
        "Proximity measurement in meters." 
    },
    { 
        "std:in.battery_level", 
        "Battery level as a percentage (0-100%)." 
    },
    { 
        "std:self.gcs_location", 
        "Geographic Coordinate system (GCS) location data including latitude, longitude, and altitude, in that order, as a real array." 
    },
    {
        "std:self.azimuth",
        "Azimuth angle, from north, measured in degrees between 0 degrees (inclusive) and 360 degrees (exclusive)."
    },
    {
        "std:out.humidity_relative",
        "Outside relative humidity measured in percentage."
    },
    {
        "std:in.humidity_relative",
        "Inside relative humidity measured in percentage."
    }
}};

} // namespace

#endif