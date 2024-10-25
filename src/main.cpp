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
#include <cmath>

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

double normalize(double value, double min, double max) {
    if (value < min || value > max) {
        std::cerr << "Value is outside the specified interval." << std::endl;
        return -1; // Or throw an exception
    }

    return (value - min) / (max - min);
}

void protogen_blinking_thread_function(std::shared_ptr<AppState> app_state) {
	// TODO: only run loop when the protogen is the active mode.
	
	// Proportion animation over time
	//
	//     ^
	//     |
	//     (proportion)
	// 1.0 |**********       *********
	//     |          *     *
	//     |           *   *
	//     |            * *
	//     |             *
	// 0.0 |----------------------------(time)->
	//

	static constexpr double animation_action_start_time = 4.5;
	static constexpr double animation_action_end_time = 5.0;
	static constexpr double animation_action_mid_time = (animation_action_start_time + animation_action_end_time) / 2;
	static_assert(animation_action_start_time < animation_action_end_time);
	
	
	while(!interrupt_received) {
		double delay_seconds = 1.0 / app_state->frameRate();

		const auto system_time = std::chrono::system_clock::now();
		const auto time_epoch = system_time.time_since_epoch();
		const double time_epoch_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(time_epoch).count() / 1000.0;
		const double animation_time = std::fmod(time_epoch_seconds, animation_action_end_time);

		Proportion eye_openness = Proportion::make(1.0).value();
		if(animation_time < animation_action_start_time) {
			eye_openness = Proportion::make(1.0).value();
		} else if(animation_action_start_time <= animation_time && animation_time <= animation_action_mid_time) {
			eye_openness = Proportion::make(std::lerp(1.0, 0.0, normalize(animation_time, animation_action_start_time, animation_action_mid_time))).value();
		} else if(animation_action_mid_time <= animation_time && animation_time <= animation_action_end_time) {
			eye_openness = Proportion::make(std::lerp(0.0, 1.0, normalize(animation_time, animation_action_mid_time, animation_action_end_time))).value();
		} else {
			eye_openness = Proportion::make(1.0).value();
		}
		app_state->protogenHeadState().setEyeOpenness(eye_openness);

		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int64_t>(delay_seconds * 1000)));
	}
}

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

std::optional<std::filesystem::path> getResourcesDir() {
	// Try local resources directory first.
	const auto current_directory = std::filesystem::current_path();
	const auto local_resources_dir = current_directory / std::filesystem::path("resources");
	if(std::filesystem::is_directory(local_resources_dir)) {
		std::cout << "Found 'resources' locally at: " << local_resources_dir << std::endl;
		return {local_resources_dir};
	}

	// Try installed resources.
	const auto installed_resources_dir = std::filesystem::path(std::string(PROTOGEN_RESOURCES_INSTALL_DIR));
	if(std::filesystem::is_directory(installed_resources_dir)) {
		std::cout << "Found 'resources' installed at: " << installed_resources_dir << std::endl;
		return {installed_resources_dir};
	}

	return {};
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
	Magick::InitializeMagick(*argv);
	
	const auto potential_resources_dir = getResourcesDir();
	if(!potential_resources_dir.has_value()) {
		std::cerr << "Could not find either your local or installed 'resources' directory." << std::endl;
		exit(1);
	}
	const std::string resources_dir = potential_resources_dir.value();
	const std::string static_web_resources_dir = (std::filesystem::path(resources_dir) / std::filesystem::path("static")).generic_string();
	const std::string html_files_dir = std::filesystem::path(resources_dir).generic_string();
	const std::string protogen_emotions_dir = (std::filesystem::path(resources_dir) / std::filesystem::path("protogen_images/eyes")).generic_string();
	const std::string static_protogen_image_path = (std::filesystem::path(resources_dir) / std::filesystem::path("protogen_images/static/nose.png")).generic_string();
	const std::string protogen_mouth_dir = (std::filesystem::path(resources_dir) / std::filesystem::path("protogen_images/mouth")).generic_string();

	auto app_state = std::shared_ptr<AppState>(new AppState());

	auto srv = std::shared_ptr<httplib::Server>(new httplib::Server());
	
	//auto mouth_openness_provider = std::unique_ptr<audio::IProportionProvider>(new audio::AudioToProportionAdapter(std::unique_ptr<audio::PcbArtistsDecibelMeter>(new audio::PcbArtistsDecibelMeter())));

	auto emotion_drawer = render::EmotionDrawer(protogen_emotions_dir);
	emotion_drawer.configWebServerToHostEmotionImages(*srv, "/protogen/head/emotion/images");

	auto data_viewer = ProtogenHeadMatrices();

	auto renderer = render::Renderer(emotion_drawer, render::MinecraftDrawer(), protogen_mouth_dir, static_protogen_image_path);

	setup_web_server(srv, app_state, html_files_dir, static_web_resources_dir);
	setup_signal_handlers();

	std::thread web_server_thread(web_server_thread_function, srv);
	std::thread protogen_blinking_thread(protogen_blinking_thread_function, app_state);
	//std::thread protogen_mouth_sync_thread(protogen_mouth_sync_thread_function, app_state, std::move(mouth_openness_provider));

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
