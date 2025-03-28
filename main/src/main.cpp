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

#include <httplib.h>

#include <Magick++.h>

#include <protogen/state/app_state.h>
#include <protogen/IRenderSurface.hpp>
#include <protogen/ICanvas.hpp>
#include <protogen/Proportion.hpp>
#include <protogen/IProtogenApp.hpp>
#include <protogen/IProportionProvider.hpp>
#include <protogen/mouth/audio.h>
#include <protogen/utils/utils.h>
#include <protogen/presentation/protogen.h>
#include <protogen/presentation/render_surface.h>
#include <protogen/presentation/sdl_render_surface.h>
#include <protogen/server/web_server.h>
#include <protogen/extensions/IExtensionFinder.h>
#include <protogen/extensions/IExtensionCheck.h>
#include <protogen/extensions/IExtensionInitializer.h>
#include <protogen/extensions/IExtensionUserDataLocator.h>
#include <protogen/extensions/IExtensionResourceDataLocator.h>
#include <protogen/extensions/initializers/BaseExtensionInitializer.h>
#include <protogen/extensions/data_locators/ExtensionHomeDirUserDataLocator.h>
#include <protogen/extensions/data_locators/ExtensionDirResourceDataLocator.h>
#include <protogen/extensions/finders/DirectoryExtensionFinder.h>
#include <protogen/extensions/checks/RequiredAttributesCheck.h>
#include <protogen/apps/ProtogenAppInitializer.h>
#include <cmake_config.h>

using namespace protogen;

volatile bool interrupt_received = false;
static void interrupt_handler([[maybe_unused]] int signal) {
	interrupt_received = true;
}

void setup_signal_handlers() {
	signal(SIGTERM, interrupt_handler);
	signal(SIGINT, interrupt_handler);
	signal(SIGABRT, interrupt_handler);
}

std::string red(const std::string& s) {
	return "\033[31m" + s + "\033[0m";
}

std::string yellow(const std::string& s) {
	return "\033[93m" + s + "\033[0m";
}

std::string green(const std::string& s) {
	return "\033[92m" + s + "\033[0m";
}

void printServiceLocationHeader(const std::string& title) {
	std::cout << "--- " << title << " ---" << std::endl;
}

void printServiceLocationSubsection(const std::string& title) {
	std::cout << "- " << title << " -" << std::endl;
}

void printServiceLocationFooter() {
	std::cout << std::endl;
}

void printNotFound() {
	//std::cout << yellow("Not found.") << std::endl;
}

std::optional<std::filesystem::path> getResourcesDir() {
	// Try local resources directory first.
	printServiceLocationSubsection("Local resources");
	const auto current_directory = std::filesystem::current_path();
	const auto local_resources_dir = current_directory / std::filesystem::path("resources");
	if(std::filesystem::is_directory(local_resources_dir)) {
		std::cout << green("Found 'resources' locally at: " + local_resources_dir.string()) << std::endl;
		return {local_resources_dir};
	} else {
		printNotFound();
	}

	// Try installed resources.
	printServiceLocationSubsection("Installed resources");
	const auto installed_resources_dir = std::filesystem::path(std::string(PROTOGEN_RESOURCES_INSTALL_DIR));
	if(std::filesystem::is_directory(installed_resources_dir)) {
		std::cout << green("Found 'resources' installed at: " + installed_resources_dir.string()) << "\033[0m" << std::endl;
		return {installed_resources_dir};
	} else {
		printNotFound();
	}

	return {};
}

std::unique_ptr<IProportionProvider> getMouthProportionProvider(std::shared_ptr<httplib::Server> srv) {
	// Try using I2C PCB artists decibel meter.
	printServiceLocationSubsection("PCB Artist's Decibel Meter");
	auto potential_pcb_artists_decibel_meter = PcbArtistsDecibelMeter::make();
	if(potential_pcb_artists_decibel_meter.has_value()) {
		std::cout << green("Audio device found: PCB Artist's Decibel Meter.") << std::endl;
		return std::unique_ptr<IProportionProvider>(new AudioToProportionAdapter(std::move(potential_pcb_artists_decibel_meter.value())));
	} else {
		printNotFound();
	}

	// try using web-based audio slider emulator.
	printServiceLocationSubsection("Web-based Audio Slider Emulator");
	std::cout << green("Audio device found: Web-based Audio Slider Emulator.") << std::endl;
	auto website_audio_provider = std::unique_ptr<IAudioProvider>(new WebsiteAudioProvider(*srv, "/protogen/head/audio-loudness"));
	auto website_proportion_provider = std::unique_ptr<IProportionProvider>(new AudioToProportionAdapter(std::move(website_audio_provider)));
	return website_proportion_provider;

	// As a fallback, use a static mouth proportion provider.
	std::cout << red("Audio device not found. Mouth will be closed at all times.") << std::endl;
	return std::unique_ptr<IProportionProvider>(new ConstantProportionProvider());
}

std::unique_ptr<IRenderSurface> getRenderSurface() {
	// Try Hub75 type led matrices.
	printServiceLocationSubsection("HUB75 interface LED Matrices");
	auto protogen_head_matrices = std::unique_ptr<ProtogenHeadMatrices>(new ProtogenHeadMatrices());
	if(protogen_head_matrices->initialize() == IExtension::Initialization::Success) {
		std::cout << green("Video device found: Protogen Head Matrices.") << std::endl;
		return protogen_head_matrices;
	} else {
		printNotFound();
	}

	// Try using SDL to display imagery. This will usually be
	// in a window in a desktop environment.
	printServiceLocationSubsection("SDL Video");
	auto sdl_device = std::unique_ptr<IRenderSurface>(new SdlRenderSurface());
	if(sdl_device->initialize() == IExtension::Initialization::Success) {
		std::cout << green("Video device found: SDL") << std::endl;
		return sdl_device;
	} else {
		printNotFound();
	}

	// As a fallback, use a fake surface.
	std::cout << red("Video device not found. You will have no way to visualize the imagery.") << std::endl;
	return std::unique_ptr<IRenderSurface>(new FakeRenderSurface());
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
	Magick::InitializeMagick(*argv);

	auto srv = std::shared_ptr<httplib::Server>(new httplib::Server());

	auto extension_user_data_locator = std::shared_ptr<IExtensionUserDataLocator>();
	auto extension_resource_data_locator = std::shared_ptr<IExtensionResourceDataLocator>();
	auto extension_base_initializer = std::shared_ptr<IExtensionInitializer>(
		new BaseExtensionInitializer(
			extension_user_data_locator,
			extension_resource_data_locator
		)
	);
	auto extension_check = std::shared_ptr<IExtensionCheck>(new RequiredAttributesCheck());
	
	printServiceLocationHeader("Display Device");
	auto data_viewer = std::shared_ptr(getRenderSurface());
	printServiceLocationFooter();

	printServiceLocationHeader("Mouth Movement Device");
	std::shared_ptr<IProportionProvider> mouth_openness_provider = getMouthProportionProvider(srv);
	printServiceLocationFooter();

	printServiceLocationHeader("Apps");
	// auto app_user_data_directory_locator = std::shared_ptr<IUserDataLocator>(new HomeDirLocator());
	// ProtogenAppInitializer app_loader(PROTOGEN_APPS_DIR, mouth_openness_provider, data_viewer->resolution(), app_user_data_directory_locator);
	// auto apps = app_loader.apps();
	// for(const auto& [app_id, app] : apps) {
	// 	const std::string app_name = app->getAttributeStore()->getAttribute(attributes::A_NAME).value_or("<no name>");
	// 	std::cout << "Found app: \"" << app_name << "\" (id: " << app_id << ")" << std::endl;
	// }
	auto app_finder = std::shared_ptr<IExtensionFinder>(new DirectoryExtensionFinder(PROTOGEN_APPS_DIR));
	auto app_checker = std::shared_ptr<IExtensionCheck>(new RequiredAttributesCheck());
	// TODO: initialize and pass in sensors.
	auto app_initializer = std::shared_ptr<IExtensionInitializer>(new ProtogenAppInitializer(extension_base_initializer, {}, data_viewer));
	auto found_apps = app_finder->find();
	std::vector<ExtensionOriginBundle> apps;
	for(auto& app : found_apps) {
		const auto initialization = app_initializer->initialize(app);
		switch(initialization) {
			case IExtensionInitializer::Initialization::Success:
				break;
			case IExtensionInitializer::Initialization::Failure:
				std::cerr << red("Failed to initialize app of id `") << app.extension->getAttributeStore()->getAttribute(attributes::A_ID).value_or("<no id>") << "`." << std::endl;
				continue;
				break;
		}
		if(!app_checker->check(app)) {
			std::cerr
				<< red("App of id `")
				<< app.extension->getAttributeStore()->getAttribute(attributes::A_ID).value_or("<no id>")
				<< "` failed to pass checks. Here is the issue: "
				<< red(app_checker->error())
				<< std::endl;
			continue;
		}
		apps.push_back(app);
	}
	printServiceLocationFooter();

	printServiceLocationHeader("Resources");
	const auto potential_resources_dir = getResourcesDir();
	if(!potential_resources_dir.has_value()) {
		std::cerr << red("Could not find either your local or installed 'resources' directory.") << std::endl;
		exit(1);
	}
	printServiceLocationFooter();

	const std::string resources_dir = potential_resources_dir.value();
	const std::string static_web_resources_dir = (std::filesystem::path(resources_dir) / std::filesystem::path("static")).generic_string();
	const std::string html_files_dir = std::filesystem::path(resources_dir).generic_string();

	auto app_state = std::shared_ptr<AppState>(new AppState());
	for(auto app : apps)
	{
		app_state->addApp(std::dynamic_pointer_cast<IProtogenApp>(app.extension));
	}

	setup_web_server(srv, app_state, html_files_dir, static_web_resources_dir);

	setup_signal_handlers();

	srv->listen("0.0.0.0", 8080);
}
