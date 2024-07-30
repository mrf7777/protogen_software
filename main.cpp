#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>

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

	ProtogenHeadState() : m_emotion(Emotion::Normal) {}
	Emotion emotion() const {
		return m_emotion;
	}
	void setEmotion(Emotion emotion) {
		m_emotion = emotion;
	}

	virtual std::string toString() const override {
		return "ProtogenHeadState{emotion: " + emotionToString(emotion()) + "}";
	}

private:
	Emotion m_emotion;
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
	
	auto ret = srv.set_mount_point("/static", "./static");
	if(!ret) {
		std::cerr << "Could not mount static directory to web server." << std::endl;
	}

	srv.listen("0.0.0.0", 8080);
}
