#include <protogen/server/web_server.h>

#include <fstream>
#include <sstream>
#include <cstdlib>

namespace protogen {

void setup_web_server(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state, const std::string& html_files_dir, const std::string& static_files_dir, const EmotionDrawer& emotion_drawer) {
	srv->set_logger([=](const auto&, auto&){
	});

	srv->set_error_handler([=](const auto& req, auto& res){
		std::cerr << "Error while handling request: " << req.method << " " << req.path << std::endl;
		res.set_content("Error handling request", "text/plain");
	});

	srv->set_exception_handler([=](const auto& req, auto& res, const std::exception_ptr e){
		try {
			std::rethrow_exception(e);
		} catch(const std::exception& e) {
			std::cerr << "Exception thrown while handling request: " << req.method << " " << req.path << std::endl;
			std::cerr << "Exception: " << e.what() << std::endl;
		} catch(...) {
			std::cerr << "Unknown exception thrown while handling request: " << req.method << " " << req.path << std::endl;
		}
		res.status = httplib::StatusCode::InternalServerError_500;
		res.set_content("Internal Server Error", "text/plain");
	});

	auto ret = srv->set_mount_point("/static", static_files_dir);
	if(!ret) {
		std::cerr << "Could not mount static directory to web server." << std::endl;
	}
	
	srv->Get("/", [html_files_dir, app_state](const auto&, auto& res){
		res.set_content(read_file_to_str(html_files_dir + "/index.html"), "text/html");
	});

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
	setup_web_server_for_apps(srv, app_state);
}

void setup_web_server_for_protogen_head(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state, const std::string& html_files_dir, const EmotionDrawer& emotion_drawer) {
	srv->Get("/protogen/face", [html_files_dir](const auto&, auto& res){
			res.set_content(read_file_to_str(html_files_dir + "/face.html"), "text/html");
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

void setup_web_server_for_apps(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state) {
	// App management endpoints
	srv->Get("/protogen/apps", [app_state](const auto&, auto& res){
		std::string app_ids_separated_by_newline;
		for(const auto& app : app_state->apps()) {
			app_ids_separated_by_newline += app.first;
			app_ids_separated_by_newline += "\n";
		}
		res.set_content(app_ids_separated_by_newline, "text/plain");
	});
	srv->Get("/protogen/apps/:appid/name", [app_state](const auto& req, auto& res){
		try {
			const auto app = app_state->apps().at(req.path_params.at("appid"));
			const std::string name = app->name().empty() ? "(No name)" : app->name();
			res.set_content(name, "text/plain");
		} catch (std::out_of_range&) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
		}
	});
	srv->Get("/protogen/apps/:appid/description", [app_state](const auto& req, auto& res){
		try {
			const auto app = app_state->apps().at(req.path_params.at("appid"));
			const std::string description = app->description().empty() ? "(No description)" : app->description();
			res.set_content(description, "text/plain");
		} catch (std::out_of_range&) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
		}
	});
	srv->Get("/protogen/apps/:appid/thumbnail", [app_state](const auto& req, auto& res){
		try {
			// TODO: should this path building be abstracted closer to apps somehow?
			const auto app = app_state->apps().at(req.path_params.at("appid"));
			std::string thumbnail_path;
			if(app->thumbnail().empty()) {
				thumbnail_path = "/static/images/no_thumbnail.png";
			} else {
				thumbnail_path = "/apps/" + app->id() + app->thumbnail();
			}
			res.set_content(thumbnail_path, "text/plain");
		} catch (std::out_of_range&) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
		}
	});
	srv->Get("/protogen/apps/:appid/active", [app_state](const auto& req, auto& res){
		try {
			const auto active_app = app_state->getActiveApp();
			if(active_app != nullptr) {
				if(active_app->id() == req.path_params.at("appid")) {
					res.set_content("true", "text/plain");
				} else {
					res.set_content("false", "text/plain");
				}
			} else {
				res.set_content("false", "text/plain");
			}
		} catch (std::out_of_range&) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
		}
	});
	srv->Put("/protogen/apps/:appid/active", [app_state](const auto& req, auto&){
		const auto app_id = req.path_params.at("appid");
		app_state->setActiveApp(app_id);
	});
	srv->Get("/protogen/apps/active", [app_state](const auto&, auto& res){
		const auto active_app = app_state->getActiveApp();
		if(active_app != nullptr) {
			res.set_content(active_app->id(), "text/plain");
		} else {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
		}
	});
	srv->Get("/protogen/apps/:appid/homepage", [app_state](const auto& req, auto& res){
		try {
			const auto app = app_state->apps().at(req.path_params.at("appid"));
			// TODO: should this path building be abstracted closer to apps somehow?
			const std::string homepage_path = "/apps/" + app->id() + app->mainPage();
			res.set_content(homepage_path, "text/plain");
		} catch (std::out_of_range&) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
		}
	});

	// Proxy requests to their respective app web servers.
	std::vector<std::pair<std::string, int>> app_ids_and_ports;
	for(const auto& app : app_state->apps()) {
		if(app.second->webPort() != -1) {
			app_ids_and_ports.push_back(std::make_pair(app.first, app.second->webPort()));
		}
	}

	std::ostringstream nginx_config;

	nginx_config << "# This file has been autogenerated by the core protogen software.\n\n";
	nginx_config << "events {\n";
	nginx_config << "  worker_connections 1024;\n";
	nginx_config << "}\n\n";

	nginx_config << "http {\n";
	nginx_config << "  server {\n";
	nginx_config << "    listen 0.0.0.0:80;\n\n";
	for(const auto& app_id_and_port : app_ids_and_ports) {
		const std::string app_id = app_id_and_port.first;
		const int app_port = app_id_and_port.second;
		const std::string app_url_path = "apps/" + app_id;
		nginx_config << "    # App id: " << app_id << "\n";
		nginx_config << "    location /" << app_url_path << "/ {\n";
		nginx_config << "      proxy_pass http://0.0.0.0:" << app_port << "/;\n";
		nginx_config << "    }\n";
		nginx_config << "    location = /" << app_url_path << "/ {\n";
		nginx_config << "      proxy_pass http://0.0.0.0/:" << app_port << "/;\n";
		nginx_config << "    }\n\n";
	}
	nginx_config << "    location / {\n";
	nginx_config << "      proxy_pass http://0.0.0.0:8080;\n";
	nginx_config << "    }\n";
	nginx_config << "    location = / {\n";
	nginx_config << "      proxy_pass http://0.0.0.0:8080/;\n";
	nginx_config << "    }\n";
	nginx_config << "  }\n";
	nginx_config << "}\n";

	//// Update nginx config file.
	{
		std::ofstream nginx_config_file("/etc/nginx/nginx.conf", std::ios::out | std::ios::trunc);
		nginx_config_file << nginx_config.str() << std::flush;
	}

	//// reload nginx with new config.
	const int nginx_reload_code = std::system("nginx -s reload");
	if(nginx_reload_code != 0) {
		std::cerr << "Could not reload nginx. Return code was " << nginx_reload_code << ". I need to have permission to send a signal to nginx. Try running me with sudo or as root." << std::endl;
		exit(1);
	}
}

} // namespace