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

#include <protogen/Proportion.hpp>
#include <protogen/IProportionProvider.hpp>

namespace protogen {

class IAudioProvider {
public:
	virtual ~IAudioProvider() = default;
	virtual double audioLevel() const = 0;
	virtual double min() const = 0;
	virtual double max() const = 0;
};


class ConstantProportionProvider : public IProportionProvider {
public:
	ConstantProportionProvider();
	virtual Proportion proportion() const override;
};

class AudioToProportionAdapter : public IProportionProvider {
public:
	AudioToProportionAdapter(std::unique_ptr<IAudioProvider> audio_provider);

	virtual Proportion proportion() const override;
private:
	std::unique_ptr<IAudioProvider> m_audioProvider;
};

class WebsiteAudioProvider final : public IAudioProvider {
public:
	/**
	 * Pass in an httplib webserver and a path that is not taken.
	 * This will configure the passed-in webserver with another PUT
	 * method path that when called with the body of a decimal number,
	 * will update this audio provider.
	 */
	WebsiteAudioProvider(httplib::Server& srv, const std::string& url_path);

	virtual double audioLevel() const override;
	virtual double min() const override;
	virtual double max() const override;
private:
	double m_currentLevel;
	mutable std::mutex m_mutex;
};

class PcbArtistsDecibelMeter : public IAudioProvider {
public:
	static std::optional<std::unique_ptr<PcbArtistsDecibelMeter>> make();
	virtual double audioLevel() const override;
	virtual double min() const override;
	virtual double max() const override;
private:
	class ConstructionException : public std::exception {
	public:
		ConstructionException();
	};
	PcbArtistsDecibelMeter();
	bool setTimeAverageMilliseconds(uint16_t miliseconds);

	static std::optional<uint8_t> readI2cByte(int i2c_file, uint8_t i2c_register);

	static bool writeI2cByte(int i2c_file, uint8_t i2c_register, uint8_t byte);

	struct FileDeleter {
		void operator()(int * file) const;
	};
	std::unique_ptr<int, FileDeleter> m_i2cFile;
	
	static constexpr const char * I2C_FILE_PATH = "/dev/i2c-1";
	static constexpr uint8_t I2C_ADDRESS = 0x48;
	static constexpr uint8_t I2C_DECIBEL_REGISTER = 0x0A;
	static constexpr uint8_t I2C_TAVG_HIGH_BYTE_REGISTER = 0x07;
	static constexpr uint8_t I2C_TAVG_LOW_BYTE_REGISTER = 0x08;
};

}	// namespace

#endif
