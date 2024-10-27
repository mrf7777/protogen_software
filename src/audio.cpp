#include <audio.h>

#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

namespace audio
{

WebsiteAudioProvider::WebsiteAudioProvider(httplib::Server& srv, const std::string& url_path) : m_currentLevel(0.0) {
    srv.Put(url_path, [this](const auto& req, auto&){
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

std::optional<std::unique_ptr<PcbArtistsDecibelMeter>> PcbArtistsDecibelMeter::make()
{
    try {
        return {std::unique_ptr<PcbArtistsDecibelMeter>(new PcbArtistsDecibelMeter())};
    } catch(const ConstructionException&) {
        return {};
    }
}

PcbArtistsDecibelMeter::PcbArtistsDecibelMeter()
{
    int open_result = open(I2C_FILE_PATH, O_RDWR);
    if(open_result < 0) {
        std::cerr << "Could not open I2C: " << I2C_FILE_PATH << std::endl;
        throw ConstructionException();
    }
    m_i2cFile = std::unique_ptr<int, FileDeleter>(new int(open_result));

    int i2c_connect_result = ioctl(*m_i2cFile, I2C_SLAVE, I2C_ADDRESS);
    if(i2c_connect_result < 0) {
        std::cerr << "Could not connect to I2C device: " << std::hex << I2C_ADDRESS << ". Is it connected?" << std::endl;
        throw ConstructionException();
    }

    // Do a test read to see if the I2C device is usable.
    const auto read_attempt = readI2cByte(*m_i2cFile, I2C_DECIBEL_REGISTER);
    if(!read_attempt.has_value()) {
        std::cerr << "It seems that the I2C device found is not a PCB Artist's Decibel Meter." << std::endl;
        throw ConstructionException();
    }

    setTimeAverageMilliseconds(17);
}

double PcbArtistsDecibelMeter::audioLevel() const {
    const auto potential_audio_level = readI2cByte(*m_i2cFile, I2C_DECIBEL_REGISTER);
    if(!potential_audio_level.has_value()) {
        return -1.0;
    }

    const double audio_level = static_cast<double>(potential_audio_level.value());
    return audio_level;
}

double PcbArtistsDecibelMeter::min() const {
    return 72;
}

double PcbArtistsDecibelMeter::max() const  {
    return 103;
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
        std::cerr << "Failed to write to register: " << std::hex << i2c_register << std::endl;
        return {};
    }

    if(read(i2c_file, &data, 1) != 1) {
        std::cerr << "Failed to read register: " << std::hex << i2c_register << std::endl;
        return {};
    }
    return {data};
}

bool PcbArtistsDecibelMeter::writeI2cByte(int i2c_file, uint8_t i2c_register, uint8_t byte) {
    uint8_t data[2] = {i2c_register, byte};
    if(write(i2c_file, &data, 2) != 2) {
        std::cerr << "Failed to write to register: " << std::hex << i2c_register << std::endl;
        return false;
    }
    return true;
}

void PcbArtistsDecibelMeter::FileDeleter::operator()(int * file) const {
    if(file) {
        close(*file);
    }
}

PcbArtistsDecibelMeter::ConstructionException::ConstructionException()
    : std::exception()
{}

AudioToProportionAdapter::AudioToProportionAdapter(std::unique_ptr<IAudioProvider> audio_provider)
    : m_audioProvider(std::move(audio_provider))
{}

Proportion AudioToProportionAdapter::proportion() const
{
    const double audio_min = m_audioProvider->min();
    const double audio_max = m_audioProvider->max();
    const double audio_level_clamped = std::clamp(m_audioProvider->audioLevel(), audio_min, audio_max);

    // We need to do reverse linear interpolation. Here is the lerp formula:
    //   l = a + t(b - a)
    // Where t is in [0, 1], a is audio min, b is audio max, and l is audio level clamped.
    //
    // We want t, since this is between 0 and 1.
    //   (l - a) / (b - a) = t
    const double proportion = (audio_level_clamped - audio_min) / (audio_max - audio_min);
    return Proportion::make(proportion).value();
}

ConstantProportionProvider::ConstantProportionProvider()
{
}

Proportion ConstantProportionProvider::proportion() const
{
    return Proportion::make(0.0).value();
}

}