#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <core/Application.hpp>
#include <util/Log.hpp>
#include <system_error>
#include <iostream>

int main() {
    // Create folder st
    // Run application
	engine::Application::Init();
	try {
		engine::Application::GetInstance()->Run();
	}
	catch (const std::system_error& e) {
		LOG_CRITICAL(e.what());
		return e.code().value();
	}
	catch (const std::runtime_error& e) {
		LOG_CRITICAL(e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}