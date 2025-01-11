#ifndef PROTOGEN_ISENSOR_HPP
#define PROTOGEN_ISENSOR_HPP

#include <cstdint>
#include <string>
#include <optional>
#include <variant>
#include <vector>

// Added extra `sensor` namespace because there are a lot of types and symbols here.
namespace protogen::sensor {

using ChannelReal = double;
using ChannelBool = bool;
using ChannelString = std::string;
using ChannelByte = uint8_t;
using ChannelInt = int32_t;
using ChannelByteArray = std::vector<uint8_t>;
using ChannelIntArray = std::vector<int32_t>;
using ChannelRealArray = std::vector<double>;
using ChannelStringArray = std::vector<std::string>;
using ChannelValue = std::variant<
    ChannelReal,
    ChannelBool,
    ChannelString,
    ChannelByte,
    ChannelInt,
    ChannelByteArray,
    ChannelIntArray,
    ChannelRealArray,
    ChannelStringArray
>;

/**
 * An interface which represents a sensor that can be attached to a protogen.
 * 
 * A sensor is a device that can read data from the environment and provide
 * that data to the protogen. This data is queried from the apps on the protogen.
 * 
 * Sensors can be used to measure a variety of things such as temperature,
 * sound, light, distance, and more. Each sensor has a unique identifier,
 * a name, a description, and a set of channels that it can read from.
 * A sensor can have multiple channels. Each channel is intended to measure
 * a specific property of the environment. There are standard channels that
 * are pre-defined for common properties such as temperature, sound, location,
 * etc.
 * 
 * Custom channels can be defined by the sensor. The sensor is responsible
 * for documenting the properties of the custom channels it provides. Please
 * make sure there is not already a standard channel that fits your needs.
 * By using standard channels, you can ensure that your sensor is compatible
 * with a wide variety of apps.
 */
class ISensor {
public:
    enum class Initialization {
        Success,
        Failure,
    };

    virtual ~ISensor() = default;

    /**
     * Initialize the sensor. This is called once when the sensor is first
     * attached to the protogen. This is called before any other methods
     * of the sensor. If the sensor fails to initialize, no other methods 
     * will be called.
     */
    virtual Initialization initialize() = 0;
    /**
     * The id of the sensor. This is a unique identifier for the sensor.
     * This is used to identify the sensor in the system.
     * 
     * The id should be a lowercase alphanumeric string, with underscores and with no spaces.
     */
    virtual std::string id() const = 0;
    /**
     * The name of the sensor. This is a human-readable name of the sensor.
     */
    virtual std::string name() const = 0;
    /**
     * A description of the sensor. This is a human-readable description of the sensor.
     * This should describe what the sensor does and what it measures.
     * Please document any custom channels that you defined.
     */
    virtual std::string description() const = 0;
    /**
     * The channels that the sensor provides. This is a list of the channels that
     * the sensor can read from. Each channel is a string that identifies the
     * property that the channel measures. The channel strings should be unique
     * and should not contain spaces.
     */
    virtual std::vector<std::string> channels() const = 0;
    /**
     * Read a value from a channel. Use the `ISensor::channels` method to
     * get what channels are available for the sensor.
     * The type of the value is determined by the channel that is read. This
     * should be documented in either the standard channels or the custom
     * channels that the sensor provides.
     * 
     * If the channel is not found or an error occurs, an empty optional is returned.
     */
    virtual std::optional<ChannelValue> read(const std::string& channel) = 0;
};

/*
 * Pre-defined standard sensor channels.
 * 
 * Each pre-defined channel specifies the following properties. Some are optional:
 * - Object of measure (required)
 * - The property it measures (required)
 * - Its units (required)
 * - Its range
 * - Its resolution
 * - Its accuracy
 * - Its precision
 * 
 * Given a pre-defined channel, a call to `ISensor::read` with the channel can be
 * expected to return a value that conforms the the specified properties of the
 * pre-defined channel read.
 */

// Measures the internal temperature of the protogen in celsius with type ChannelReal.
static const char * CHANNEL_INTERNAL_TEMPERATURE = "internal_temperature";
// Measures the external temperature of the protogen in celsius with type ChannelReal.
static const char * CHANNEL_EXTERNAL_TEMPERATURE = "external_temperature";
// Measures the internal "loudness" of the protogen in decibels with type ChannelReal.
static const char * CHANNEL_INTERNAL_LOUDNESS  = "internal_loudness";
// Measures the external "loudness" of the protogen in decibels with type ChannelReal.
static const char * CHANNEL_EXTERNAL_LOUDNESS  = "external_loudness";
// Measures the location of the protogen in latitude and longitude in WGS84 with type ChannelRealArray of size 2.
// The first element is the latitude and the second element is the longitude in degrees.
static const char * CHANNEL_LAT_LON_WGS84 = "lat_lon_wgs84";
// Measures the azimuth of the protogen from the north in degrees with type ChannelReal.
static const char * CHANNEL_AZIMUTH_FROM_NORTH = "azimuth_from_north";
// Measures the altitude of the protogen in WGS84 in meters with type ChannelReal.
static const char * CHANNEL_ALTITUDE_WGS84 = "altitude_wgs84";
// Measures the distance to the front object in meters with type ChannelReal.
// Returns negative value if no object is detected.
// If a sensor has this channel, consider also having channel "is_booped"
static const char * CHANNEL_DISTANCE_TO_FRONT_OBJECT = "distance_to_front_object";
// Returns value of ChannelBool true if the protogen is booped, false otherwise.
// A protogen is considered "booped" if it is currently being touched on its nose.
static const char * CHANNEL_IS_BOOPED = "is_booped";
// Measures the confidence of the presence of certain vowels inside the protogen with type ChannelRealArray of size 5.
// The following vowels are measured in order: a, e, i, o, u.
// "a" as in ah, "e" as in eh, "i" as in ee or itch, "o" as in oh, "u" as in oo.
// The values are in the range [0, 1] where 0 means no confidence and 1 means high confidence.
static const char * CHANNEL_INTERNAL_VOWEL_CONFIDENCE = "internal_vowel_confidence";

} // namespace

#endif