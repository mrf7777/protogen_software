#include "common_headers.hpp"

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


void setup_web_server_for_protogen_head(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state) {
	srv->Get("/", [](const httplib::Request & req, httplib::Response & res){
			res.set_content(read_file_to_str("./index.html"), "text/html");
	});
	srv->Put("/protogen/head/start", [app_state](const auto& req, auto& res){
			app_state->setMode(AppState::Mode::ProtogenHead);
	});
	srv->Get("/protogen/head/emotion/all", [app_state](const auto& req, auto& res){
			res.set_content(ProtogenHeadState::emotionsSeperatedByNewline(), "text/plain");
	});
	srv->Get("/protogen/head/emotion", [app_state](const auto& req, auto& res){
			const auto emotion = app_state->protogenHeadState().emotion();
			const auto emotion_string = ProtogenHeadState::emotionToString(emotion);
			res.set_content(emotion_string, "text/plain");
	});
	srv->Put("/protogen/head/emotion", [app_state](const auto& req, auto& res){
			const auto emotion = ProtogenHeadState::emotionFromString(req.body);
			app_state->protogenHeadState().setEmotion(emotion);
	});
	srv->Get("/protogen/head/blank", [app_state](const auto& req, auto& res){
			const auto blank = app_state->protogenHeadState().blank();
			const auto blank_string = blank ? "true" : "false";
			res.set_content(blank_string, "text/plain");
	});
	srv->Put("/protogen/head/blank", [app_state](const auto& req, auto& res){
			const auto blank = req.body == "true";
			app_state->protogenHeadState().setBlank(blank);
	});
	srv->Get("/protogen/head/brightness/all", [app_state](const auto& req, auto& res){
			res.set_content(ProtogenHeadState::brightnessLevelsSeperatedByNewline(), "text/plain");
	});
	srv->Get("/protogen/head/brightness", [app_state](const auto& req, auto& res){
			const auto brightness = app_state->protogenHeadState().brightness();
			const auto brightness_string = ProtogenHeadState::brightnessToString(brightness);
			res.set_content(brightness_string, "text/plain");
	});
	srv->Put("/protogen/head/brightness", [app_state](const auto& req, auto& res){
			const auto brightness = ProtogenHeadState::stringToBrightness(req.body);
			app_state->protogenHeadState().setBrightness(brightness);
	});
}

void setup_web_server_for_minecraft(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state) {
	srv->Get("/protogen/minecraft", [app_state](const auto& req, auto& res){
		res.set_content(read_file_to_str("./minecraft.html"), "text/html");
	});
	srv->Get("/protogen/minecraft/interface", [app_state](const auto& req, auto& res){
		res.set_content(read_file_to_str("./minecraft_interface.html"), "text/html");
	});
	srv->Put("/protogen/minecraft/world/generate", [app_state](const auto& req, auto& res){
		const std::size_t seed = std::hash<std::string>{}(req.body);
		const auto world = mc::BlockMatrixGenerator(32, 128).generate(seed);
		app_state->minecraftState().blockMatrix() = world;
	});
	
	srv->Get("/protogen/minecraft/players", [app_state](const auto& req, auto& res){
		res.set_content(app_state->minecraftState().playersSeperatedByNewline(), "text/plain");
	});
	srv->Put("/protogen/minecraft/players/:player", [app_state](const auto& req, auto& res){
		app_state->minecraftState().addNewPlayer(req.path_params.at("player"));
	});
	srv->Delete("/protogen/minecraft/players/:player", [app_state](const auto& req, auto& res){
		app_state->minecraftState().removePlayer(req.path_params.at("player"));
	});
	srv->Post("/protogen/minecraft/players/:player/move", [app_state](const auto& req, auto& res){
		const auto player_id = req.path_params.at("player");
		const auto move_direction = MinecraftPlayerState::stringToCursorDirection(req.body);
		app_state->minecraftState().accessPlayer(player_id, [move_direction](MinecraftPlayerState& player_state){
			player_state.moveCursor(move_direction);
		});
	});
	srv->Post("/protogen/minecraft/players/:player/place_block", [app_state](const auto& req, auto& res){
		const auto player_id = req.path_params.at("player");
		MinecraftPlayerState::CursorPos player_cursor;
		mc::Block player_block;
		app_state->minecraftState().accessPlayer(player_id, [&player_cursor, &player_block](MinecraftPlayerState& player_state){
			player_cursor = player_state.cursor();
			player_block = player_state.selectedBlock();
		});
		app_state->minecraftState().blockMatrix().set(
			player_cursor.first,
			player_cursor.second,
			player_block
		);
	});
	srv->Put("/protogen/minecraft/players/:player/block", [app_state](const auto& req, auto& res){
		const auto player_id = req.path_params.at("player");
		const auto block = mc::Block::fromString(req.body);
		app_state->minecraftState().accessPlayer(player_id, [block](MinecraftPlayerState& player_state){
			player_state.setSelectedBlock(block);
		});
	});

	srv->Get("/protogen/minecraft/blocks", [app_state](const auto& req, auto& res){
		res.set_content(mc::Block::allBlocksSeperatedByNewline(), "text/plain");
	});
	srv->Get("/protogen/minecraft/blocks/:block/color", [app_state](const auto& req, auto& res){
		const auto block = mc::Block::fromString(req.body);
		const auto block_color = app_state->minecraftState().blockColorProfile()(block);
		const auto color_hex = mc::colorHexCodeFromColor(block_color);
		res.set_content(color_hex, "text/plain");
	});
}

void setup_web_server(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state) {
	srv->set_logger([=](const auto& req, const auto& res){
	});
	
	srv->Get("/protogen/mode", [app_state](const auto& req, auto& res){
			const auto mode = app_state->mode();
			const auto mode_string = AppState::modeToString(mode);
			res.set_content(mode_string, "text/plain");
	});
	srv->Put("/protogen/mode", [app_state](const auto& req, auto& res){
			const auto mode = AppState::stringToMode(req.body);
			app_state->setMode(mode);
	});

	setup_web_server_for_protogen_head(srv, app_state);
	setup_web_server_for_minecraft(srv, app_state);
}

void setup_signal_handlers() {
	signal(SIGTERM, interrupt_handler);
	signal(SIGINT, interrupt_handler);
	signal(SIGABRT, interrupt_handler);
}

int main(int argc, char *argv[]) {
	Magick::InitializeMagick(*argv);
	
	auto app_state = std::shared_ptr<AppState>(new AppState());

	auto srv = std::shared_ptr<httplib::Server>(new httplib::Server());
	setup_web_server(srv, app_state);
	
	auto decibel_module_audio_provider = std::unique_ptr<audio::PcbArtistsDecibelMeter>(new audio::PcbArtistsDecibelMeter());

	auto emotion_drawer = EmotionDrawer();
	emotion_drawer.configWebServerToHostEmotionImages(*srv, "/protogen/head/emotion/images");

	auto data_viewer = std::unique_ptr<ProtogenHeadMatrices>(new ProtogenHeadMatrices(argc, argv, std::move(decibel_module_audio_provider), emotion_drawer));

	auto ret = srv->set_mount_point("/static", "./static");
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
