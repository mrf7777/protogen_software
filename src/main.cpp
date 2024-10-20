#include <common_headers.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>
#include <signal.h>

#include <graphics.h>
#include <canvas.h>
#include <led-matrix.h>
#include <pixel-mapper.h>

#include <httplib.h>

#include <Magick++.h>

#include <images.h>
#include <audio.h>
#include <utils.h>
#include <protogen.h>
#include <web_server.h>
#include <cmake_config.h>

volatile bool interrupt_received = false;
static void interrupt_handler([[maybe_unused]] int signal) {
	interrupt_received = true;
}

void web_server_thread_function(std::shared_ptr<httplib::Server> server) {
	server->listen("0.0.0.0", 8080);
}

void protogen_blinking_thread_function(std::shared_ptr<AppState> app_state) {
	static constexpr int BLINKING_OFF_MILLISECONDS = 7000;
	static constexpr int BLINKING_ON_MILLISECONDS = 100;
	while(!interrupt_received) {
		std::this_thread::sleep_for(std::chrono::milliseconds(BLINKING_OFF_MILLISECONDS));
		app_state->protogenHeadState().setForceBlink(true);
		std::this_thread::sleep_for(std::chrono::milliseconds(BLINKING_ON_MILLISECONDS));
		app_state->protogenHeadState().setForceBlink(false);
	}
}

void setup_signal_handlers() {
	signal(SIGTERM, interrupt_handler);
	signal(SIGINT, interrupt_handler);
	signal(SIGABRT, interrupt_handler);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
	Magick::InitializeMagick(*argv);
	
	auto app_state = std::shared_ptr<AppState>(new AppState());

	auto srv = std::shared_ptr<httplib::Server>(new httplib::Server());
	setup_web_server(srv, app_state);
	
	auto decibel_module_audio_provider = std::unique_ptr<audio::PcbArtistsDecibelMeter>(new audio::PcbArtistsDecibelMeter());

	auto emotion_drawer = EmotionDrawer();
	emotion_drawer.configWebServerToHostEmotionImages(*srv, "/protogen/head/emotion/images");

	auto data_viewer = std::unique_ptr<ProtogenHeadMatrices>(new ProtogenHeadMatrices(std::move(decibel_module_audio_provider), emotion_drawer));

	auto ret = srv->set_mount_point("/static", "./resources/static");
	if(!ret) {
		std::cerr << "Could not mount static directory to web server." << std::endl;
	}

	setup_signal_handlers();

	std::thread web_server_thread(web_server_thread_function, srv);
	std::thread protogen_blinking_thread(protogen_blinking_thread_function, app_state);

	int FPS;
	while(!interrupt_received) {
		FPS = app_state->frameRate();
		data_viewer->viewData(*app_state);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
	}

	// clean up
	srv->stop();
}
