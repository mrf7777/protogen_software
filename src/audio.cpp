#include <audio.h>

#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

namespace audio
{

WebsiteAudioProvider::WebsiteAudioProvider(httplib::Server& srv, const std::string& url_path) : m_currentLevel(0.0) {
    srv.Put(url_path, [this](const auto& req, auto& res){
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->m_currentLevel = std::stod(req.body);
    });
}

double WebsiteAudioProvider::audioLevel() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentLevel;
}

double WebsiteAudioProvider::min() const {
    return 0.0;
}

double WebsiteAudioProvider::max() const {
    return 100.0;
}

PcbArtistsDecibelMeter::PcbArtistsDecibelMeter() {
    int open_result = open(I2C_FILE_PATH, O_RDWR);
    if(open_result < 0) {
        std::cerr << "Could not open I2C: " << I2C_FILE_PATH << std::endl;
        exit(1);
    }
    m_i2cFile = std::unique_ptr<int, FileDeleter>(new int(open_result));

    int i2c_connect_result = ioctl(*m_i2cFile, I2C_SLAVE, I2C_ADDRESS);
    if(i2c_connect_result < 0) {
        std::cerr << "Could not connect to I2C device: " << I2C_ADDRESS << ". Is it connected?" << std::endl;
    }

    setTimeAverageMilliseconds(17);
}

double PcbArtistsDecibelMeter::audioLevel() const {
    const auto potential_audio_level = readI2cByte(*m_i2cFile, I2C_DECIBEL_REGISTER);
    if(!potential_audio_level.has_value()) {
        return -1.0;
    }

    const double audio_level = static_cast<double>(potential_audio_level.value());
    std::cout << "Audio level: " << audio_level << std::endl;
    return audio_level;
}

double PcbArtistsDecibelMeter::min() const {
    return 72;
}

double PcbArtistsDecibelMeter::max() const  {
    return 103;
}

bool PcbArtistsDecibelMeter::setTimeAverageMilliseconds(uint16_t mili) {
    uint8_t low = mili & 0xff;
    uint8_t high = (mili>>8) & 0xff;
    if(!writeI2cByte(*m_i2cFile, I2C_TAVG_HIGH_BYTE_REGISTER, high))
        return false;
    if(!writeI2cByte(*m_i2cFile, I2C_TAVG_LOW_BYTE_REGISTER, low))
        return false;
    return true;
}

std::optional<uint8_t> PcbArtistsDecibelMeter::readI2cByte(int i2c_file, uint8_t i2c_register) {
    uint8_t data = i2c_register;
    if(write(i2c_file, &data, 1) != 1) {
        std::cerr << "Failed to write to register: " << i2c_register << std::endl;
        return {};
    }

    if(read(i2c_file, &data, 1) != 1) {
        std::cerr << "Failed to read register: " << i2c_register << std::endl;
        return {};
    }
    return {data};
}

bool PcbArtistsDecibelMeter::writeI2cByte(int i2c_file, uint8_t i2c_register, uint8_t byte) {
    uint8_t data[2] = {i2c_register, byte};
    if(write(i2c_file, &data, 2) != 2) {
        std::cerr << "Failed to write to register: " << i2c_register << std::endl;
        return false;
    }
    return true;
}

void PcbArtistsDecibelMeter::FileDeleter::operator()(int * file) const {
    if(file) {
        close(*file);
    }
}

}