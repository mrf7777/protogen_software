#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <mutex>
#include <signal.h>

#include <graphics.h>
#include <canvas.h>
#include <led-matrix.h>
#include <pixel-mapper.h>

#include <httplib.h>

#include <Magick++.h>

#include "images.h"
#include "audio.h"
#include "utils.h"
#include "protogen.h"

volatile bool interrupt_received = false;
static void interrupt_handler(int signal) {
	interrupt_received = true;
}

void web_server_thread_function(std::shared_ptr<httplib::Server> server) {
	server->listen("0.0.0.0", 8080);
}

int main(int argc, char *argv[]) {
	Magick::InitializeMagick(*argv);
	
	auto app_state = std::shared_ptr<AppState>(new AppState());

	auto srv = std::shared_ptr<httplib::Server>(new httplib::Server());

	srv->set_logger([=](const auto& req, const auto& res){
	});

	srv->Get("/", [](const httplib::Request & req, httplib::Response & res){
			res.set_content(read_file_to_str("./index.html"), "text/html");
	});
	srv->Get("/protogen/head/emotion/all", [app_state](const auto& req, auto& res){
			res.set_content(ProtogenHeadState::emotionsSeperatedByNewline(), "text/plain");
	});
	srv->Put("/protogen/head/emotion", [app_state](const auto& req, auto& res){
			const auto emotion = ProtogenHeadState::emotionFromString(req.body);
			app_state->protogenHeadState().setEmotion(emotion);
	});	

	// use phone as a microphone/audio provider
	auto web_audio_provider = std::unique_ptr<audio::WebsiteAudioProvider>(new audio::WebsiteAudioProvider(*srv, "/protogen/head/audio-loudness"));

	auto emotion_drawer = EmotionDrawer();
	emotion_drawer.configWebServerToHostEmotionImages(*srv, "/protogen/head/emotion/images");

	auto data_viewer = std::unique_ptr<ProtogenHeadMatrices>(new ProtogenHeadMatrices(argc, argv, std::move(web_audio_provider), emotion_drawer));

	auto ret = srv->set_mount_point("/static", "./static");
	if(!ret) {
		std::cerr << "Could not mount static directory to web server." << std::endl;
	}

	// signals
	signal(SIGTERM, interrupt_handler);
	signal(SIGINT, interrupt_handler);
	signal(SIGABRT, interrupt_handler);

	// start threads
	std::thread web_server_thread(web_server_thread_function, srv);
	
	static const int FPS = 30;
	while(!interrupt_received) {
		data_viewer->viewData(*app_state);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
	}

	// clean up
	srv->stop();
	data_viewer->clear();
}
