#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <utility>

#include <httplib.h>

#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

namespace audio {

class IAudioProvider {
public:
	virtual ~IAudioProvider() = default;
	virtual double audioLevel() const = 0;
	virtual double min() const = 0;
	virtual double max() const = 0;
};

class WebsiteAudioProvider final : public IAudioProvider {
public:
	/**
	 * Pass in an httplib webserver and a path that is not taken.
	 * This will configure the passed-in webserver with another PUT
	 * method path that when called with the body of a decimal number,
	 * will update this audio provider.
	 */
	WebsiteAudioProvider(httplib::Server& srv, const std::string& url_path) : m_currentLevel(0.0) {
		srv.Put(url_path, [this](const auto& req, auto& res){
			std::lock_guard<std::mutex> lock(this->m_mutex);
			this->m_currentLevel = std::stod(req.body);
		});
	}
	virtual double audioLevel() const override {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_currentLevel;
	}
	virtual double min() const override {
		return 0.0;
	}
	virtual double max() const override {
		return 100.0;
	}
private:
	double m_currentLevel;
	mutable std::mutex m_mutex;
};

class PcbArtistsDecibelMeter : public IAudioProvider {
public:
	PcbArtistsDecibelMeter() {
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
	virtual double audioLevel() const override {
		const auto potential_audio_level = readI2cByte(*m_i2cFile, I2C_DECIBEL_REGISTER);
		if(!potential_audio_level.has_value()) {
			return -1.0;
		}

		const double audio_level = static_cast<double>(potential_audio_level.value());
		std::cout << "Audio level: " << audio_level << std::endl;
		return audio_level;
	}
	virtual double min() const override {
		return 72;
	}
	virtual double max() const override {
		return 103;
	}
private:
	bool setTimeAverageMilliseconds(uint16_t mili) {
		uint8_t low = mili & 0xff;
		uint8_t high = (mili>>8) & 0xff;
		if(!writeI2cByte(*m_i2cFile, I2C_TAVG_HIGH_BYTE_REGISTER, high))
			return false;
		if(!writeI2cByte(*m_i2cFile, I2C_TAVG_LOW_BYTE_REGISTER, low))
			return false;
		return true;
	}

	static std::optional<uint8_t> readI2cByte(int i2c_file, uint8_t i2c_register) {
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

	static bool writeI2cByte(int i2c_file, uint8_t i2c_register, uint8_t byte) {
		uint8_t data[2] = {i2c_register, byte};
		if(write(i2c_file, &data, 2) != 2) {
			std::cerr << "Failed to write to register: " << i2c_register << std::endl;
			return false;
		}
		return true;
	}

	struct FileDeleter {
		void operator()(int * file) const {
			if(file) {
				close(*file);
			}
		}
	};
	std::unique_ptr<int, FileDeleter> m_i2cFile;
	
	static constexpr const char * I2C_FILE_PATH = "/dev/i2c-1";
	static constexpr uint8_t I2C_ADDRESS = 0x48;
	static constexpr uint8_t I2C_DECIBEL_REGISTER = 0x0A;
	static constexpr uint8_t I2C_TAVG_HIGH_BYTE_REGISTER = 0x07;
	static constexpr uint8_t I2C_TAVG_LOW_BYTE_REGISTER = 0x08;
};

}

#endif
