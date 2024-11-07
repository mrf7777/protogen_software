#include <protogen/server/web_server.h>

namespace protogen {

void setup_web_server(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state, const std::string& html_files_dir, const std::string& static_files_dir, const EmotionDrawer& emotion_drawer, const Apps& apps) {
	srv->set_logger([=](const auto&, auto&){
	});

	auto ret = srv->set_mount_point("/static", static_files_dir);
	if(!ret) {
		std::cerr << "Could not mount static directory to web server." << std::endl;
	}
	
	srv->Get("/protogen/mode", [app_state](const auto&, auto& res){
			const auto mode = app_state->mode();
			const auto mode_string = AppState::modeToString(mode);
			res.set_content(mode_string, "text/plain");
	});
	srv->Put("/protogen/mode", [app_state](const auto& req, auto&){
			const auto mode = AppState::stringToMode(req.body);
			app_state->setMode(mode);
	});

	setup_web_server_for_protogen_head(srv, app_state, html_files_dir, emotion_drawer);
	setup_web_server_for_minecraft(srv, app_state, html_files_dir);
	setup_web_server_for_apps(srv, apps);
}

void setup_web_server_for_protogen_head(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state, const std::string& html_files_dir, const EmotionDrawer& emotion_drawer) {
	srv->Get("/", [html_files_dir](const auto&, auto& res){
			res.set_content(read_file_to_str(html_files_dir + "/index.html"), "text/html");
	});
	srv->Put("/protogen/head/start", [app_state](const auto&, auto&){
			app_state->setMode(AppState::Mode::ProtogenHead);
	});
	srv->Get("/protogen/head/emotion/all", [app_state, emotion_drawer](const auto&, auto& res){
			res.set_content(emotion_drawer.emotionsSeparatedByNewline(), "text/plain");
	});
	srv->Get("/protogen/head/emotion", [app_state](const auto&, auto& res){
			const auto emotion = app_state->protogenHeadState().emotion();
			res.set_content(emotion, "text/plain");
	});
	srv->Put("/protogen/head/emotion", [app_state](const auto& req, auto&){
			app_state->protogenHeadState().setEmotion(req.body);
	});
	srv->Get("/protogen/head/blank", [app_state](const auto&, auto& res){
			const auto blank = app_state->protogenHeadState().blank();
			const auto blank_string = blank ? "true" : "false";
			res.set_content(blank_string, "text/plain");
	});
	srv->Put("/protogen/head/blank", [app_state](const auto& req, auto&){
			const auto blank = req.body == "true";
			app_state->protogenHeadState().setBlank(blank);
	});
	srv->Get("/protogen/head/brightness/all", [app_state](const auto&, auto& res){
			res.set_content(ProtogenHeadState::brightnessLevelsSeparatedByNewline(), "text/plain");
	});
	srv->Get("/protogen/head/brightness", [app_state](const auto&, auto& res){
			const auto brightness = app_state->protogenHeadState().brightness();
			const auto brightness_string = ProtogenHeadState::brightnessToString(brightness);
			res.set_content(brightness_string, "text/plain");
	});
	srv->Put("/protogen/head/brightness", [app_state](const auto& req, auto&){
			const auto brightness = ProtogenHeadState::stringToBrightness(req.body);
			app_state->protogenHeadState().setBrightness(brightness);
	});
}

void setup_web_server_for_minecraft(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state, const std::string& html_files_dir) {
	srv->Get("/protogen/minecraft", [app_state, html_files_dir](const auto&, auto& res){
		res.set_content(read_file_to_str(html_files_dir + "/minecraft.html"), "text/html");
	});
	srv->Get("/protogen/minecraft/interface", [app_state, html_files_dir](const auto&, auto& res){
		res.set_content(read_file_to_str(html_files_dir + "/minecraft_interface.html"), "text/html");
	});
	srv->Put("/protogen/minecraft/world/generate", [app_state](const auto& req, auto&){
		const std::size_t seed = std::hash<std::string>{}(req.body);
		const auto world = BlockMatrixGenerator(32, 128).generate(seed);
		app_state->minecraftState().blockMatrix() = world;
	});
	
	srv->Get("/protogen/minecraft/players", [app_state](const auto&, auto& res){
		res.set_content(app_state->minecraftState().playersSeparatedByNewline(), "text/plain");
	});
	srv->Put("/protogen/minecraft/players/:player", [app_state](const auto& req, auto&){
		app_state->minecraftState().addNewPlayer(req.path_params.at("player"));
	});
	srv->Delete("/protogen/minecraft/players/:player", [app_state](const auto& req, auto&){
		app_state->minecraftState().removePlayer(req.path_params.at("player"));
	});
	srv->Post("/protogen/minecraft/players/:player/move", [app_state](const auto& req, auto&){
		const auto player_id = req.path_params.at("player");
		const auto move_direction = MinecraftPlayerState::stringToCursorDirection(req.body);
		app_state->minecraftState().accessPlayer(player_id, [move_direction](MinecraftPlayerState& player_state){
			player_state.moveCursor(move_direction);
		});
	});
	srv->Post("/protogen/minecraft/players/:player/place_block", [app_state](const auto& req, auto&){
		const auto player_id = req.path_params.at("player");
		MinecraftPlayerState::CursorPos player_cursor;
		Block player_block;
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
	srv->Put("/protogen/minecraft/players/:player/block", [app_state](const auto& req, auto&){
		const auto player_id = req.path_params.at("player");
		const auto block = Block::fromString(req.body);
		app_state->minecraftState().accessPlayer(player_id, [block](MinecraftPlayerState& player_state){
			player_state.setSelectedBlock(block);
		});
	});

	srv->Get("/protogen/minecraft/blocks", [app_state](const auto&, auto& res){
		res.set_content(Block::allBlocksSeparatedByNewline(), "text/plain");
	});
	srv->Get("/protogen/minecraft/blocks/:block/color", [app_state](const auto& req, auto& res){
		const auto block = Block::fromString(req.path_params.at("block"));
		const auto block_color = app_state->minecraftState().blockColorProfile()(block);
		const auto color_hex = colorHexCodeFromColor(block_color);
		res.set_content(color_hex, "text/plain");
	});
}

void setup_web_server_for_apps(std::shared_ptr<httplib::Server> srv, const Apps& apps) {
	for(const auto& app : apps) {
		setup_web_server_for_app(srv, app.second);
	}

	// App management endpoints
	srv->Get("/protogen/apps", [apps](const auto&, auto& res){
		std::string app_ids_separated_by_newline;
		for(const auto& app : apps) {
			app_ids_separated_by_newline += app.first;
			app_ids_separated_by_newline += "\n";
		}
		res.set_content(app_ids_separated_by_newline, "text/plain");
	});
	srv->Get("/protogen/apps/:appid/name", [apps](const auto& req, auto& res){
		try {
			res.set_content(apps.at(req.path_params.at("appid"))->name(), "text/plain");
		} catch (std::out_of_range&) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
		}
	});
	srv->Get("/protogen/apps/:appid/description", [apps](const auto& req, auto& res){
		try {
			res.set_content(apps.at(req.path_params.at("appid"))->description(), "text/plain");
		} catch (std::out_of_range&) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
		}
	});
	srv->Get("/protogen/apps/:appid/homepage", [apps](const auto& req, auto& res){
		try {
			const auto app = apps.at(req.path_params.at("appid"));
			const std::string homepage_path = "/apps/" + app->id() + app->homePage();
			res.set_content(homepage_path, "text/plain");
		} catch (std::out_of_range&) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
		}
	});
}

void setup_web_server_for_app(std::shared_ptr<httplib::Server> srv, std::shared_ptr<IProtogenApp> app){
	using HttpMethod = IProtogenApp::HttpMethod;

	const std::string app_id = app->id();

	// Setup app endpoints.
	for(const auto& endpoint : app->serverEndpoints()) {
		const std::string endpoint_full_path = "/apps/" + app_id + endpoint.first.relativePath;
		switch(endpoint.first.method) {
		case HttpMethod::Get:
			srv->Get(endpoint_full_path, endpoint.second);
			break;
		case HttpMethod::Post:
			srv->Post(endpoint_full_path, endpoint.second);
			break;
		case HttpMethod::Put:
			srv->Put(endpoint_full_path, endpoint.second);
			break;
		case HttpMethod::Delete:
			srv->Delete(endpoint_full_path, endpoint.second);
			break;
		case HttpMethod::Patch:
			srv->Patch(endpoint_full_path, endpoint.second);
			break;
		case HttpMethod::Options:
			srv->Options(endpoint_full_path, endpoint.second);
			break;
		}
	}

	// Setup app static file hosting.
	const std::string static_files_rel_dir = app->staticFilesDirectory();
	// TODO: pass in base path for apps
	const std::string static_files_absolute_dir = "/usr/local/share/protogen/apps/" + app_id + "/resources/" + static_files_rel_dir;
	if(!static_files_rel_dir.empty()) {
		const std::string static_files_mount_point = "/apps/" + app_id + app->staticFilesPath();
		srv->set_mount_point(static_files_mount_point, static_files_absolute_dir);
	}
}

} // namespace