#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <util/Log.hpp>
#include <core/Application.hpp>
#include <system_error>
#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>
#include <algorithm>

int main() {
    std::filesystem::create_directory("worlds");

    std::string option;
    while (option != "l" && option != "c") {
        std::cout << "Load or create world? (L/C): ";
        std::cin >> option;
        std::transform(option.begin(), option.end(), option.begin(), ::tolower);
    }
    std::cin.ignore();

    std::string worldName;

    if (option == "l") {
        std::cout << "Enter world name: ";
        std::getline(std::cin, worldName);
    }

    if (option == "c") {
        while (true) {
            std::cout << "Enter world name: ";
            std::getline(std::cin, worldName);

            bool worldCreated = std::filesystem::create_directory(fmt::format("worlds/{}", worldName));

            if (worldCreated) {
                break;
            } else {
                std::cout << "World already exists! ";
            }
        }

        std::filesystem::create_directory(fmt::format("worlds/{}/chunks", worldName));

         // get seed
        const size_t MAXIMUM_CHARS = 32;
        static char buffer[MAXIMUM_CHARS + 1]; // An extra for the terminating nul character.
        std::cout << "Enter seed: ";
        std::cin.getline(buffer, MAXIMUM_CHARS, '\n');
        const std::string seedString(buffer);
        std::hash<std::string> hasher;
        siv::PerlinNoise::seed_type seed = static_cast<siv::PerlinNoise::seed_type>(hasher(seedString));

        // create world.yml
        YAML::Node worldNode;
        worldNode["seed"] = seed;
        std::ofstream fWorldOut(fmt::format("worlds/{}/world.yml", worldName), std::ofstream::trunc);
        fWorldOut << worldNode;
        fWorldOut.close();

        // create player.yml
        YAML::Node playerNode;
        playerNode["position"] = engine::Vec3<float>(0.0f,0.0f,0.0f);
        playerNode["yaw"] = -90.0f;
        playerNode["pitch"] = 0.0f;
        std::ofstream fPlayerOut(fmt::format("worlds/{}/player.yml", worldName), std::ofstream::trunc);
        fPlayerOut << playerNode;
        fPlayerOut.close();
    }

    engine::Log::Init();

	engine::Application::Init();
	try {
		engine::Application::GetInstance()->Run(worldName.c_str());
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