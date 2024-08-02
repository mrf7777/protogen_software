#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <mutex>

#include <graphics.h>
#include <canvas.h>
#include <led-matrix.h>
#include <pixel-mapper.h>

#include <httplib.h>

#include <Magick++.h>

#include "Images.h"

static void writeImagetoCanvas(const Magick::Image &img, rgb_matrix::Canvas* canvas) {
	for(std::size_t y = 0; y < img.rows(); ++y) {
		for(std::size_t x = 0; x < img.columns(); ++x) {
			const Magick::Color& c = img.pixelColor(x, y);
			if(c.alphaQuantum() < 255) {
				canvas->SetPixel(
					x, y,
					ScaleQuantumToChar(c.redQuantum()),
					ScaleQuantumToChar(c.greenQuantum()),
					ScaleQuantumToChar(c.blueQuantum())
				);
			}
		}
	}
}

template<typename Data>
class IViewData {
public:
	virtual ~IViewData() = default;
	virtual void viewData(const Data& data) = 0;
};

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
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_emotion;
	}
	void setEmotion(Emotion emotion) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_emotion = emotion;
	}
	RGBColor mouthColor() const {
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_mouthColor;
	}
	void setMouthColor(const RGBColor& color) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_mouthColor = color;
	}
	void setEyeColor(const RGBColor& color) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_eyeColor = color;
	}

	virtual std::string toString() const override {
		std::lock_guard<std::mutex> lock(m_mutex);
		return "ProtogenHeadState{emotion: " + emotionToString(m_emotion) + ", eyeColor: " + m_eyeColor.toString() + ", mouthColor: " + m_mouthColor.toString() + "}";
	}

private:
	Emotion m_emotion;
	RGBColor m_mouthColor;
	RGBColor m_eyeColor;
	mutable std::mutex m_mutex;
};

class AppState final : public IToString {
public:
	AppState() {}
	ProtogenHeadState& protogenHeadState() {
		return m_protogenHeadState;
	}
	const ProtogenHeadState& protogenHeadState() const {
		return m_protogenHeadState;
	}

	virtual std::string toString() const override {
		return "AppState{protogenHeadState: " + m_protogenHeadState.toString() + "}";
	}
private:
	ProtogenHeadState m_protogenHeadState;
};

class ProtogenHeadMatrices final : public IViewData<AppState> {
public:
	ProtogenHeadMatrices(int argc, char *argv[]) {
		rgb_matrix::RGBMatrix::Options options;
		options.rows = 32;
		options.cols = 64;
		options.chain_length = 2;
		options.brightness = 100;
		options.hardware_mapping = "adafruit-hat";

		rgb_matrix::RuntimeOptions runtime_opts;
		runtime_opts.drop_privileges = -1;

		m_matrix = std::unique_ptr<rgb_matrix::RGBMatrix>(
			rgb_matrix::RGBMatrix::CreateFromOptions(
				options, runtime_opts
			)
		);
		m_matrix->Clear();
	};
	~ProtogenHeadMatrices() {
		m_matrix->Clear();
	}
	virtual void viewData(const AppState& data) override {
		std::cout << data.toString() << std::endl;
		m_matrix->Clear();
		m_matrix->SetPixel(0, 0, 255, 255, 255);
		m_matrix->SetPixel(127, 0, 255, 255, 255);
		m_matrix->SetPixel(127, 63, 255, 255, 255);
		m_matrix->SetPixel(0, 63, 255, 255, 255);
		m_matrix->SetPixel(data.protogenHeadState().mouthColor().r()/2, 31, 255, 255, 255);
	}
private:
	std::unique_ptr<rgb_matrix::RGBMatrix> m_matrix;
};

std::string read_file_to_str(const std::string& filename) {
	std::ifstream file(filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void data_viewer_thread_function(std::shared_ptr<AppState> app_state, std::unique_ptr<IViewData<AppState>> data_viewer) {
	static const int FPS = 30;
	while(true) {
		std::cout << "Draw!" << std::endl;
		data_viewer->viewData(*app_state);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
	}
}

int main(int argc, char *argv[]) {
	Magick::InitializeMagick(*argv);
	
	auto app_state = std::shared_ptr<AppState>(new AppState());

	auto data_viewer = std::unique_ptr<IViewData<AppState>>(new ProtogenHeadMatrices(argc, argv));

	httplib::Server srv;

	srv.set_logger([=](const auto& req, const auto& res){
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

	// start threads
	std::thread data_viewer_thread(data_viewer_thread_function, app_state, std::move(data_viewer));

	srv.listen("0.0.0.0", 8080);
}
