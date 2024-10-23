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
#include <filesystem>
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
#include <renderer.h>
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

// 
void protogen_mouth_sync_thread_function(std::shared_ptr<AppState> app_state, std::unique_ptr<audio::IProportionProvider> mouth_openness_provider) {
	// TODO: only run loop when the protogen is the active mode.
	int framerate = app_state->frameRate();
	while(!interrupt_received) {
		app_state->protogenHeadState().setMouthOpenness(mouth_openness_provider->proportion());
		framerate = app_state->frameRate();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000/framerate));
	}
}

void setup_signal_handlers() {
	signal(SIGTERM, interrupt_handler);
	signal(SIGINT, interrupt_handler);
	signal(SIGABRT, interrupt_handler);
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
	Magick::InitializeMagick(*argv);
	
	const std::string resources_dir = std::string(PROTOGEN_RESOURCES_INSTALL_DIR);
	const std::string static_web_resources_dir = (std::filesystem::path(resources_dir) / std::filesystem::path("static")).generic_string();
	const std::string html_files_dir = std::filesystem::path(resources_dir).generic_string();
	const std::string protogen_emotions_dir = (std::filesystem::path(resources_dir) / std::filesystem::path("protogen_images/eyes")).generic_string();
	const std::string static_protogen_image_path = (std::filesystem::path(resources_dir) / std::filesystem::path("protogen_images/static/nose.png")).generic_string();
	const std::string protogen_mouth_dir = (std::filesystem::path(resources_dir) / std::filesystem::path("protogen_images/mouth")).generic_string();

	auto app_state = std::shared_ptr<AppState>(new AppState());

	auto srv = std::shared_ptr<httplib::Server>(new httplib::Server());
	setup_web_server(srv, app_state, html_files_dir);
	
	auto mouth_openness_provider = std::unique_ptr<audio::IProportionProvider>(new audio::AudioToProportionAdapter(std::unique_ptr<audio::PcbArtistsDecibelMeter>(new audio::PcbArtistsDecibelMeter())));


	auto emotion_drawer = render::EmotionDrawer(protogen_emotions_dir);
	emotion_drawer.configWebServerToHostEmotionImages(*srv, "/protogen/head/emotion/images");

	auto data_viewer = ProtogenHeadMatrices();

	auto renderer = render::Renderer(emotion_drawer, render::MinecraftDrawer(), protogen_mouth_dir, static_protogen_image_path);

	// TODO: move to webserver setup
	auto ret = srv->set_mount_point("/static", static_web_resources_dir);
	if(!ret) {
		std::cerr << "Could not mount static directory to web server." << std::endl;
	}

	setup_signal_handlers();

	std::thread web_server_thread(web_server_thread_function, srv);
	std::thread protogen_blinking_thread(protogen_blinking_thread_function, app_state);
	std::thread protogen_mouth_sync_thread(protogen_mouth_sync_thread_function, app_state, std::move(mouth_openness_provider));

	int FPS;
	while(!interrupt_received) {
		FPS = app_state->frameRate();
		
		data_viewer.drawFrame([&](rgb_matrix::Canvas& canvas) {
			renderer.render(*app_state, canvas);
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(1000/FPS));
	}

	// clean up
	srv->stop();
}
