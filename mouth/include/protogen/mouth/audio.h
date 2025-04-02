#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <stdexcept>
#include <optional>

#include <httplib.h>

#include <protogen/ISensor.hpp>
#include <protogen/Proportion.hpp>
#include <protogen/StandardAttributeStore.hpp>
#include <protogen/IProportionProvider.hpp>

namespace protogen {

class PcbArtistsDecibelMeter : public sensor::ISensor {
public:
	PcbArtistsDecibelMeter();
	Initialization initialize() override;

	std::optional<std::string> getAttribute(const std::string& key) const override;
	std::vector<std::string> listAttributes() const override;
	bool hasAttribute(const std::string& key) const override;
	SetAttributeResult setAttribute(const std::string& key, const std::string& value) override;
	RemoveAttributeResult removeAttribute(const std::string& key) override;
	std::vector<ChannelInfo> channels() const override;
	ReadResult read(const std::string& channel_id) override;

private:
	bool setTimeAverageMilliseconds(uint16_t miliseconds);
	static std::optional<uint8_t> readI2cByte(int i2c_file, uint8_t i2c_register);
	static bool writeI2cByte(int i2c_file, uint8_t i2c_register, uint8_t byte);

	struct FileDeleter {
		void operator()(int * file) const;
	};
	std::unique_ptr<int, FileDeleter> m_i2cFile;
	StandardAttributeStore m_attributes;
	
	static constexpr const char * I2C_FILE_PATH = "/dev/i2c-1";
	static constexpr uint8_t I2C_ADDRESS = 0x48;
	static constexpr uint8_t I2C_DECIBEL_REGISTER = 0x0A;
	static constexpr uint8_t I2C_TAVG_HIGH_BYTE_REGISTER = 0x07;
	static constexpr uint8_t I2C_TAVG_LOW_BYTE_REGISTER = 0x08;
};

}	// namespace

#endif
