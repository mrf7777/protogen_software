#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <memory>

#include <utils.h>
#include <app_state.h>

#include <httplib.h>

void setup_web_server(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state);
void setup_web_server_for_protogen_head(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state);
void setup_web_server_for_minecraft(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state);

#endif