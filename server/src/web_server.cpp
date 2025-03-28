#include <protogen/server/web_server.h>

#include <fstream>
#include <sstream>
#include <cstdlib>

namespace protogen {

void setup_web_server(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state, const std::string& html_files_dir, const std::string& static_files_dir) {
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

	setup_web_server_for_apps(srv, app_state);
}

void setup_web_server_for_apps(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state) {
	// App management endpoints
	srv->Get("/protogen/apps", [app_state](const auto&, auto& res){
		std::string app_ids_separated_by_newline;
		for(const auto& appId : app_state->appIds()) {
			app_ids_separated_by_newline += appId;
			app_ids_separated_by_newline += "\n";
		}
		res.set_content(app_ids_separated_by_newline, "text/plain");
	});
	srv->Get("/protogen/apps/:appid/name", [app_state](const auto& req, auto& res){
		const auto app = app_state->getApp(req.path_params.at("appid"));
		if(!app.has_value()) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
			return;
		}
		const auto name = app.value()->getAttributeStore()->getAttribute(attributes::A_NAME).value_or("");
		res.set_content(name, "text/plain");
	});
	srv->Get("/protogen/apps/:appid/description", [app_state](const auto& req, auto& res){
		const auto app = app_state->getApp(req.path_params.at("appid"));
		if(!app.has_value()) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
			return;
		}
		const auto description = app.value()->getAttributeStore()->getAttribute(attributes::A_DESCRIPTION).value_or("");
		res.set_content(description, "text/plain");
	});
	srv->Get("/protogen/apps/:appid/thumbnail", [app_state](const auto& req, auto& res){
		// TODO: should this path building be abstracted closer to apps somehow?
		const auto app_id = req.path_params.at("appid");
		const auto app = app_state->getApp(app_id);
		if(!app.has_value()) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
			return;
		}
		std::string thumbnail_path;
		const auto app_thumbnail_path = app.value()->getAttributeStore()->getAttribute(attributes::A_THUMBNAIL);
		if(!app_thumbnail_path.has_value()) {
			thumbnail_path = "/static/images/no_thumbnail.png";
		} else {
			thumbnail_path = "/apps/" + app_id + app_thumbnail_path.value();
		}
		res.set_content(thumbnail_path, "text/plain");
	});
	srv->Get("/protogen/apps/:appid/active", [app_state](const auto& req, auto& res){
		const auto active_app_id = app_state->getActiveAppId();
		if(!active_app_id.has_value()) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
			return;
		}
		const auto app_id_to_compare = req.path_params.at("appid");
		if(active_app_id.value() == app_id_to_compare) {
			res.set_content("true", "text/plain");
		} else {
			res.set_content("false", "text/plain");
		}
	});
	srv->Put("/protogen/apps/:appid/active", [app_state](const auto& req, auto&){
		const auto app_id = req.path_params.at("appid");
		app_state->setActiveApp(app_id);
	});
	srv->Get("/protogen/apps/active", [app_state](const auto&, auto& res){
		const auto active_app_id = app_state->getActiveAppId();
		if(active_app_id.has_value()) {
			res.set_content(active_app_id.value(), "text/plain");
		} else {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
		}
	});
	srv->Get("/protogen/apps/:appid/homepage", [app_state](const auto& req, auto& res){
		const auto app_id = req.path_params.at("appid");
		const auto app = app_state->getApp(app_id);
		if(!app.has_value()) {
			res.status = httplib::StatusCode::NotFound_404;
			res.set_content("", "text/plain");
			return;
		}
		const auto app_mainpage_path = app.value()->getAttributeStore()->getAttribute(attributes::A_MAIN_PAGE).value_or("");
		// TODO: should this path building be abstracted closer to apps somehow?
		const std::string homepage_path = "/apps/" + app_id + app_mainpage_path;
		res.set_content(homepage_path, "text/plain");
	});

	// Proxy requests to their respective app web servers.
	std::vector<std::pair<std::string, int>> app_ids_and_ports;
	for(const auto& app_id : app_state->appIds()) {
		std::string app_web_port_string;
		try {
			app_web_port_string = app_state->getApp(app_id).value()->getAttributeStore()->getAttribute(attributes::A_WEB_PORT).value_or("-1");
			const int app_web_port = std::stoi(app_web_port_string);
			if(app_web_port != -1) {
				app_ids_and_ports.push_back(std::make_pair(app_id, app_web_port));
			}
		} catch(const std::invalid_argument& e) {
			std::cerr << "Could not convert app web port to integer for app with id `" << app_id << "`. Port was \"" << app_web_port_string << "\"" << std::endl;
		} catch(const std::out_of_range& e) {
			std::cerr << "Could not convert app web port to integer for app with id `" << app_id << "`. Port was \"" << app_web_port_string << "\"" << std::endl;
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