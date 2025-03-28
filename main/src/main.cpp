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
#include <protogen/sensors/SensorOriginBundle.h>
#include <protogen/sensors/SensorsProvider.h>
#include <protogen/apps/ProtogenAppInitializer.h>
#include <protogen/apps/AppsProvider.h>
#include <cmake_config.h>

using namespace protogen;

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

	auto extension_user_data_locator = std::shared_ptr<IExtensionUserDataLocator>(new ExtensionHomeDirUserDataLocator(".protogen", "userdata"));
	auto extension_resource_data_locator = std::shared_ptr<IExtensionResourceDataLocator>(new ExtensionDirResourceDataLocator("resources"));
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

	printServiceLocationHeader("Sensor Devices");
	auto sensor_finder = std::shared_ptr<IExtensionFinder>(new DirectoryExtensionFinder(PROTOGEN_SENSORS_DIR));
	auto sensor_check = extension_check;
	auto sensor_initializer = extension_base_initializer;
	auto found_sensors = sensor_finder->find();
	auto sensors_provider = std::shared_ptr<SensorsProvider>(new SensorsProvider(sensor_finder, sensor_initializer, sensor_check));
	auto sensors = sensors_provider->loadSensors();
	printServiceLocationFooter();

	printServiceLocationHeader("Apps");
	auto app_finder = std::shared_ptr<IExtensionFinder>(new DirectoryExtensionFinder(PROTOGEN_APPS_DIR));
	auto app_checker = extension_check;
	std::vector<std::shared_ptr<sensor::ISensor>> sensors_vector;
	for(auto& sensor : sensors) {
		sensors_vector.push_back(sensor.second.sensor);
	}
	auto app_initializer = std::shared_ptr<IExtensionInitializer>(new ProtogenAppInitializer(extension_base_initializer, sensors_vector, data_viewer));
	auto apps_provider = std::shared_ptr<AppsProvider>(new AppsProvider(app_finder, app_initializer, app_checker));
	auto apps = apps_provider->loadApps();
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
		app_state->addApp(std::dynamic_pointer_cast<IProtogenApp>(app.second.app));
	}

	setup_web_server(srv, app_state, html_files_dir, static_web_resources_dir);

	srv->listen("0.0.0.0", 8080);
}
