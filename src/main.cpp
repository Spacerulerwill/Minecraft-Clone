#define STB_IMAGE_IMPLEMENTATION

#include <util/Log.hpp>
#include <core/Application.hpp>
#include <system_error>

int main() {
    engine::Log::Init();

	engine::Application::Init();
	try {
		engine::Application::GetInstance()->Run();
	}
	catch (const std::system_error& e) {
		LOG_CRITICAL(e.what());
        glfwTerminate();
		return e.code().value();
	}
	catch (const std::runtime_error& e) {
		LOG_CRITICAL(e.what());
        glfwTerminate();
		return EXIT_FAILURE;
	}
    glfwTerminate();
	return EXIT_SUCCESS;
}