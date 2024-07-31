#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>

#include <graphics.h>
#include <canvas.h>
#include <led-matrix.h>
#include <pixel-mapper.h>

#include <httplib.h>

class IToString {
public:
	~IToString() = default;
	virtual std::string toString() const = 0;
};

class RGBColor : public IToString {
public:
	RGBColor(uint8_t r, uint8_t g, uint8_t b) : m_r(r), m_g(g), m_b(b) {}
	RGBColor(std::string hex) {
		if(hex.at(0) == '#') {
			hex.erase(0, 1);
		}
		while(hex.length() != 6) {
			hex += "0";
		}
		std::cout << hex << std::endl;
		uint8_t r, g, b;
		sscanf(hex.data(), "%02hhx%02hhx%02hhx", &r, &g, &b);
		m_r = r;
		m_g = g;
		m_b = b;
	}
	uint8_t r() const { return m_r; }
	uint8_t g() const { return m_g; }
	uint8_t b() const { return m_b; }

	virtual std::string toString() const override {
		return "RGBColor{r: " + std::to_string(m_r) + ", g: " + std::to_string(m_g) + ", b: " + std::to_string(m_b) + "}";
	}
private:
	uint8_t m_r;
	uint8_t m_g;
	uint8_t m_b;
};

class ProtogenHeadState final : public IToString {
public:
	enum class Emotion {
		Normal,
		Angry,
		Happy,
		Sad,
	};

	static Emotion emotionFromString(const std::string& s) {
		if(s == "normal")
			return Emotion::Normal;
		else if(s == "angry")
			return Emotion::Angry;
		else if(s == "happy")
			return Emotion::Happy;
		else if(s == "sad")
			return Emotion::Sad;
		else
			return Emotion::Normal;
	}

	static std::string emotionToString(Emotion e) {
		switch(e) {
		case Emotion::Normal:
			return "normal";
		case Emotion::Angry:
			return "angry";
		case Emotion::Happy:
			return "happy";
		case Emotion::Sad:
			return "sad";
		}
	}

	ProtogenHeadState()
		: m_emotion(Emotion::Normal),
		m_mouthColor(RGBColor(0, 255, 0)),
	        m_eyeColor(RGBColor(0, 255, 0))	
	{}

	Emotion emotion() const {
		return m_emotion;
	}
	void setEmotion(Emotion emotion) {
		m_emotion = emotion;
	}
	void setMouthColor(const RGBColor& color) {
		m_mouthColor = color;
	}
	void setEyeColor(const RGBColor& color) {
		m_eyeColor = color;
	}

	virtual std::string toString() const override {
		return "ProtogenHeadState{emotion: " + emotionToString(emotion()) + ", eyeColor: " + m_eyeColor.toString() + ", mouthColor: " + m_mouthColor.toString() + "}";
	}

private:
	Emotion m_emotion;
	RGBColor m_mouthColor;
	RGBColor m_eyeColor;
};

class AppState final : public IToString {
public:
	AppState() {}
	ProtogenHeadState& protogenHeadState() {
		return m_protogenHeadState;
	}

	virtual std::string toString() const override {
		return "AppState{protogenHeadState: " + m_protogenHeadState.toString() + "}";
	}
private:
	ProtogenHeadState m_protogenHeadState;
};

std::string read_file_to_str(const std::string& filename) {
	std::ifstream file(filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

int main() {
	std::cout << "Hello, World!" << std::endl;

	auto app_state = std::shared_ptr<AppState>(new AppState());

	const auto options = rgb_matrix::RGBMatrix::Options();
	const auto runtime_options = rgb_matrix::RuntimeOptions();
	auto matrix = std::unique_ptr<rgb_matrix::RGBMatrix>(rgb_matrix::RGBMatrix::CreateFromOptions(
		options,
		runtime_options
	));

	httplib::Server srv;

	srv.set_logger([=](const auto& req, const auto& res){
		std::cout << "New app state: " << app_state->toString() << std::endl;
	});

	srv.Get("/", [](const httplib::Request & req, httplib::Response & res){
			res.set_content(read_file_to_str("./index.html"), "text/html");
	});

	srv.Put("/protogen/head/emotion", [app_state](const auto& req, auto& res){
			const auto emotion = ProtogenHeadState::emotionFromString(req.body);
			app_state->protogenHeadState().setEmotion(emotion);
	});
	srv.Put("/protogen/head/mouth/color", [app_state](const auto& req, auto& res){
			app_state->protogenHeadState().setMouthColor(req.body);
	});
	srv.Put("/protogen/head/eye/color", [app_state](const auto& req, auto& res){
			app_state->protogenHeadState().setEyeColor(req.body);
	});
	
	auto ret = srv.set_mount_point("/static", "./static");
	if(!ret) {
		std::cerr << "Could not mount static directory to web server." << std::endl;
	}

	srv.listen("0.0.0.0", 8080);
}
