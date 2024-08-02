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

#include "images.h"
#include "audio.h"
#include "utils.h"
#include "protogen.h"

void data_viewer_thread_function(std::shared_ptr<AppState> app_state, std::unique_ptr<IViewData<AppState>> data_viewer) {
	static const int FPS = 30;
	while(true) {
		data_viewer->viewData(*app_state);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
	}
}

int main(int argc, char *argv[]) {
	Magick::InitializeMagick(*argv);
	
	auto app_state = std::shared_ptr<AppState>(new AppState());

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

	// use phone as a microphone/audio provider
	auto web_audio_provider = std::unique_ptr<audio::WebsiteAudioProvider>(new audio::WebsiteAudioProvider(srv, "/protogen/head/audio-loudness"));

	auto data_viewer = std::unique_ptr<IViewData<AppState>>(new ProtogenHeadMatrices(argc, argv, std::move(web_audio_provider)));

	auto ret = srv.set_mount_point("/static", "./static");
	if(!ret) {
		std::cerr << "Could not mount static directory to web server." << std::endl;
	}

	// start threads
	std::thread data_viewer_thread(data_viewer_thread_function, app_state, std::move(data_viewer));

	srv.listen("0.0.0.0", 8080);
}
