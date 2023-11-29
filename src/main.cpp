#define STB_IMAGE_IMPLEMENTATION

#include <core/Application.hpp>
#include <system_error>
#include <util/Log.hpp>

int main() {
    engine::Log::Init();
    try {
        engine::Application application;
        application.Run();
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