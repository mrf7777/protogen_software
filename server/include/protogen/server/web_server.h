#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <memory>

#include <protogen/utils/utils.h>
#include <protogen/state/app_state.h>
#include <protogen/rendering/renderer.h>

#include <httplib.h>

namespace protogen {

void setup_web_server(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state, const std::string& html_files_dir, const std::string& static_files_dir, const EmotionDrawer& emotion_drawer);
void setup_web_server_for_protogen_head(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state, const std::string& html_files_dir, const EmotionDrawer& emotion_drawer);
void setup_web_server_for_minecraft(std::shared_ptr<httplib::Server> srv, std::shared_ptr<AppState> app_state, const std::string& html_files_dir);

}   // namespace

#endif