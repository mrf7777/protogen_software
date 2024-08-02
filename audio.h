#ifndef AUDIO_H
#define AUDIO_H

#include <string>
#include <mutex>

#include <httplib.h>

namespace audio {

class IAudioProvider {
public:
	virtual ~IAudioProvider() = default;
	virtual double audioLevel() const = 0;
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
private:
	double m_currentLevel;
	mutable std::mutex m_mutex;
};

}

#endif
