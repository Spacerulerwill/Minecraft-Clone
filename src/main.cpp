/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GLFW/glfw3.h>
#include <core/Game.hpp>
#include <world/World.hpp>
#include <util/Log.hpp>
#include <util/IO.hpp>
#include <core/SoundEngine.hpp>
#include <string>
#include <iostream>
#include <limits>
#include <filesystem>
#include <functional>
#include <fmt/format.h>
#include <PerlinNoise.hpp>

#define RED "\x1b[0;31m"
#define GREEN "\x1b[0;32m"
#define RESET "\x1b[0m"

struct MenuOptionResult {
    bool success;
    std::string msg;
};

MenuOptionResult create_world() {
	// Get name of new world
	std::string worldName;
	std::cout << "Enter new world name: ";
	std::cin >> worldName;
	std::string worldDirectory = fmt::format("worlds/{}", worldName);

	// Check it doesn't exist
	if (std::filesystem::is_directory(worldDirectory)) {
        return MenuOptionResult{
            .success = false,
            .msg = fmt::format("World {} already exists", worldName)
        };
	}

	// ask user for seed
	std::string seedString;
	std::cout << "Enter world seed: ";
	std::cin >> seedString;
	std::hash<std::string> hasher;
    auto seed = static_cast<siv::PerlinNoise::seed_type>(hasher(seedString));

	// If it doesn't exist, create world
	std::filesystem::create_directory(worldDirectory);
	std::filesystem::create_directory(fmt::format("{}/chunk_stacks", worldDirectory));
	WorldSave worldSave {
		.seed = seed,
		.elapsedTime = 0.0
	};
	WriteStructToDisk(fmt::format("{}/world.data", worldDirectory), worldSave);
	PlayerSave playerSave {
		.pos = Vec3{0.0f, 1000.0f, 0.0f},
		.pitch = 0.0f,
		.yaw = -90.0f
	};
	WriteStructToDisk(fmt::format("{}/player.data", worldDirectory), playerSave);
    return MenuOptionResult{
        .success = true,
        .msg = fmt::format("Created world {}!", worldName)
    };
}

MenuOptionResult delete_world() {
	// Get name of world to delete
    for (const auto& entry : std::filesystem::directory_iterator("worlds")) {
		if (entry.is_directory()){
			std::cout << "* " << entry.path().filename().string() << std::endl;
		}
	}

	std::string worldName;
	std::cout << "Enter name of world to delete: ";
	std::cin >> worldName;
	std::string worldDirectory = fmt::format("worlds/{}", worldName);

	// Check world to delete exists
	if (!std::filesystem::is_directory(worldDirectory)) {
        return MenuOptionResult{
            .success = false,
            .msg = fmt::format("World {} doesn't exist", worldName)
        };
	}

	// Remove all its files
	std::filesystem::remove_all(worldDirectory);
    return MenuOptionResult{
        .success = true,
        .msg = fmt::format("Deleted world {}!", worldName)
    };
}

MenuOptionResult load_world() {
	// list worlds available
	for (const auto& entry : std::filesystem::directory_iterator("worlds")) {
		if (entry.is_directory()){
			std::cout << "* " << entry.path().filename().string() << std::endl;
		}
	}
	
	// get user choice
	std::string worldName;
	std::cout << "Enter world name: ";
	std::cin >> worldName;
	std::string worldDirectory = fmt::format("worlds/{}", worldName);
	if (!std::filesystem::is_directory(worldDirectory)) {
        return MenuOptionResult{
            .success = false,
            .msg = fmt::format("World {} does not exist!", worldName)
        };
	}

	try {
        std::cout << GREEN << fmt::format("Started playing world {}", worldName) << RESET << std::endl;

		Game game;
		game.Run(worldDirectory);

        return MenuOptionResult{
            .success = true,
            .msg = fmt::format("Finished playing world {}", worldName)
        };
	} catch (const WorldCorruptionException& e) {
        return MenuOptionResult{
            .success = false,
            .msg = e.what()
        };
	}
}

int main() {
	try{
        Log::GetLogger();
	    SoundEngine::PreloadGameSounds();
	    std::filesystem::create_directory("worlds");
	
        MenuOptionResult lastResult{};
		char choice;
		std::cout << R"(
  ____            __ _      
 / ___|_ __ __ _ / _| |_  _     _   
| |   | '__/ _` | |_| __|| |_ _| |_ 
| |___| | | (_| |  _| ||_   _|_   _|
 \____|_|  \__,_|_|  \__||_|   |_|  

Choose an option:
C - Create world
L - Load world
D - Delete world
Q - Quit
)";

		do {            
            if (lastResult.msg != "") {
                std::cout << (lastResult.success ? GREEN : RED) << lastResult.msg << RESET << std::endl;
            }

            std::cout << "Enter your choice: ";
			std::cin >> choice;
			choice = std::tolower(choice);

			switch (choice) {
				case 'c': {
					lastResult = create_world();
					break;
				}
				case 'l': {
					lastResult = load_world();	
					break;
				}
				case 'd': {
					lastResult = delete_world();
					break;
				}
				case 'q': {
					break;
				}
				default: {
                    lastResult = MenuOptionResult{
                        .success = false,
                        .msg = "Invalid choice!"
                    };
					break;
				}
			}
		} while (choice != 'q');
    }
    catch (const std::runtime_error& e) {
        LOG_CRITICAL(e.what());
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        LOG_CRITICAL(e.what());
        return EXIT_FAILURE;
    }
    catch (...) {
        LOG_CRITICAL("Unhandled exception caught!");
        return EXIT_FAILURE;
    }
	LOG_INFO("Exited successfully");
    return EXIT_SUCCESS;
}

/*
MIT License

Copyright (c) 2023 William Redding

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
