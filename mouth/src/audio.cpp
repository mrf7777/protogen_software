#include <protogen/mouth/audio.h>
#include <protogen/StandardAttributes.hpp>

#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

namespace protogen
{

PcbArtistsDecibelMeter::PcbArtistsDecibelMeter()
    : m_i2cFile(nullptr),
    m_attributes()
{
    m_attributes.setAttribute(attributes::A_ID, "pcb_artists_decibel_meter");
    m_attributes.setAttribute(attributes::A_NAME, "PCB Artist's Decibel Meter");
    m_attributes.setAttribute(attributes::A_DESCRIPTION, "A decibel meter that uses I2C to communicate to PCB Artist's hardware.");
    m_attributes.setAttribute(attributes::A_AUTHOR, "mrf7777");
}

IInitializable::Initialization PcbArtistsDecibelMeter::initialize() {
    int open_result = open(I2C_FILE_PATH, O_RDWR);
    if(open_result < 0) {
        std::cerr << "Could not open I2C: " << I2C_FILE_PATH << std::endl;
        return Initialization::Failure;
    }
    m_i2cFile = std::unique_ptr<int, FileDeleter>(new int(open_result));

    int i2c_connect_result = ioctl(*m_i2cFile, I2C_SLAVE, I2C_ADDRESS);
    if(i2c_connect_result < 0) {
        std::cerr << "Could not connect to I2C device: " << std::hex << static_cast<int>(I2C_ADDRESS) << ". Is it connected?" << std::endl;
        return Initialization::Failure;
    }

    // Do a test read to see if the I2C device is usable.
    const auto read_attempt = readI2cByte(*m_i2cFile, I2C_DECIBEL_REGISTER);
    if(!read_attempt.has_value()) {
        std::cerr << "It seems that the I2C device found is not a PCB Artist's Decibel Meter." << std::endl;
        return Initialization::Failure;
    }

    setTimeAverageMilliseconds(17);
    return Initialization::Success;
}

std::optional<std::string> PcbArtistsDecibelMeter::getAttribute(const std::string& key) const {
    return m_attributes.getAttribute(key);
}
std::vector<std::string> PcbArtistsDecibelMeter::listAttributes() const {
    return m_attributes.listAttributes();
}
bool PcbArtistsDecibelMeter::hasAttribute(const std::string& key) const {
    return m_attributes.hasAttribute(key);
}
attributes::IWritableAttributeStore::SetAttributeResult PcbArtistsDecibelMeter::setAttribute(const std::string& key, const std::string& value) {
    return m_attributes.setAttribute(key, value);
}
attributes::IWritableAttributeStore::RemoveAttributeResult PcbArtistsDecibelMeter::removeAttribute(const std::string& key) {
    return m_attributes.removeAttribute(key);
}
std::vector<sensor::ISensor::ChannelInfo> PcbArtistsDecibelMeter::channels() const {
    return {
        {"std:in.sound_level", "Internal sound intensity level measured in decibels (dB)."},
    };
}
sensor::ISensor::ReadResult PcbArtistsDecibelMeter::read(const std::string& channel_id) {
    if (channel_id == "std:in.sound_level") {
        const auto potential_audio_level = readI2cByte(*m_i2cFile, I2C_DECIBEL_REGISTER);
        if(!potential_audio_level.has_value()) {
            return ReadResult{
                std::chrono::system_clock::now(),
                {},
                ReadError::HardwareFailure
            };
        }
    
        const double audio_level = static_cast<double>(potential_audio_level.value());

        return ReadResult{
            std::chrono::system_clock::now(),
            {audio_level},
            {}
        };
    }
    return ReadResult{
        std::chrono::system_clock::now(),
        {},
        ReadError::ChannelNotFound
    };
}

bool PcbArtistsDecibelMeter::setTimeAverageMilliseconds(uint16_t miliseconds) {
    uint8_t low = miliseconds & 0xff;
    uint8_t high = (miliseconds>>8) & 0xff;
    if(!writeI2cByte(*m_i2cFile, I2C_TAVG_HIGH_BYTE_REGISTER, high))
        return false;
    if(!writeI2cByte(*m_i2cFile, I2C_TAVG_LOW_BYTE_REGISTER, low))
        return false;
    return true;
}

std::optional<uint8_t> PcbArtistsDecibelMeter::readI2cByte(int i2c_file, uint8_t i2c_register) {
    uint8_t data = i2c_register;
    if(write(i2c_file, &data, 1) != 1) {
        std::cerr << "Failed to write to register: " << std::hex << static_cast<int>(i2c_register) << std::endl;
        return {};
    }

    if(::read(i2c_file, &data, 1) != 1) {
        std::cerr << "Failed to read register: " << std::hex << static_cast<int>(i2c_register) << std::endl;
        return {};
    }
    return {data};
}

bool PcbArtistsDecibelMeter::writeI2cByte(int i2c_file, uint8_t i2c_register, uint8_t byte) {
    uint8_t data[2] = {i2c_register, byte};
    if(write(i2c_file, &data, 2) != 2) {
        std::cerr << "Failed to write to register: " << std::hex << static_cast<int>(i2c_register) << std::endl;
        return false;
    }
    return true;
}

void PcbArtistsDecibelMeter::FileDeleter::operator()(int * file) const {
    if(file) {
        close(*file);
    }
}

}   // namespace