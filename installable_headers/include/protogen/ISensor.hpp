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
using ChannelInt16 = int16_t;
using ChannelInt32 = int32_t;
using ChannelInt64 = int64_t;
using ChannelByteArray = std::vector<uint8_t>;
using ChannelInt32Array = std::vector<int32_t>;
using ChannelInt64Array = std::vector<int64_t>;
using ChannelRealArray = std::vector<double>;
using ChannelStringArray = std::vector<std::string>;
using ChannelValue = std::variant<
    ChannelReal,
    ChannelBool,
    ChannelString,
    ChannelByte,
    ChannelInt16,
    ChannelInt32,
    ChannelInt64,
    ChannelByteArray,
    ChannelInt32Array,
    ChannelInt64Array,
    ChannelRealArray,
    ChannelStringArray
>;

class ISensor {
public:
    enum class Initialization {
        Success,
        Failure,
    };

    virtual ~ISensor() = default;

    virtual Initialization initialize() = 0;
    virtual std::string id() const = 0;
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;
    virtual std::vector<std::string> channels() const = 0;
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