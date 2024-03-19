/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <core/Game.hpp>
#include <core/SoundEngine.hpp>
#include <opengl/Window.hpp>
#include <world/World.hpp>
#include <util/Log.hpp>
#include <util/IO.hpp>

#include <PerlinNoise.hpp>
#include <fmt/format.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <sstream>
#include <limits>
#include <filesystem>
#include <functional>
#include <stdint.h>
#include <random>
#include <limits>

#define RED "\x1b[0;31m"
#define GREEN "\x1b[0;32m"
#define RESET "\x1b[0m"

struct MenuOptionResult {
    bool success {};
    std::string msg {};
};

std::uintmax_t get_size_of_directory(const std::filesystem::directory_entry& directory_entry) {
    std::uintmax_t size = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_entry)) {
        if (std::filesystem::is_regular_file(entry)) {
            size += std::filesystem::file_size(entry);
        }
    }
    return size;
}

std::string format_bytes(std::uintmax_t bytes) {
    static const char* suffixes[] = { "B", "KB", "MB", "GB", "TB" };
    std::size_t suffixIndex = 0;

    std::uintmax_t size = bytes;
    while (size >= 1024 && suffixIndex < 4) {
        size /= 1024;
        suffixIndex++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << suffixes[suffixIndex];
    return oss.str();
}

MenuOptionResult create_world() {
    static std::random_device rd;
    static std::mt19937 gen(rd()); 
    static std::hash<std::string> seedHasher;
                                                   
    // Get name of new world
    std::string worldName;
    std::cout << "Enter new world name: ";
    std::getline(std::cin, worldName);
    if (worldName.empty()) {
        return MenuOptionResult {
            .success = false,
            .msg = "World name cannot be empty"
        };
    }
    std::string worldDirectory = fmt::format("worlds/{}", worldName);

    // Check it doesn't exist
    if (std::filesystem::is_directory(worldDirectory)) {
        return MenuOptionResult{
            .success = false,
            .msg = fmt::format("World {} already exists", worldName)
        };
    }

    // Ask user for seed
    std::string seedString;
    std::cout << "Enter world seed (leave blank for random): ";
    std::getline(std::cin, seedString);
    siv::PerlinNoise::seed_type seed;
    if (seedString.empty()) {
        std::uniform_int_distribution<siv::PerlinNoise::seed_type> distr(
            std::numeric_limits<siv::PerlinNoise::seed_type>::min(),
            std::numeric_limits<siv::PerlinNoise::seed_type>::max()
        );    
        seed = distr(gen);
    } else {
        seed = static_cast<siv::PerlinNoise::seed_type>(seedHasher(seedString));
    }

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
    std::size_t worldCount = 0;
    for (const auto& entry : std::filesystem::directory_iterator("worlds")) {
        if (entry.is_directory()){
            worldCount++;
            std::cout << "* " << entry.path().filename().string() << ": " << format_bytes(get_size_of_directory(entry)) << std::endl;
        }
    }

    if (worldCount == 0) {
        return MenuOptionResult {
            .success = false,
            .msg = "You have no worlds to delete"
        };
    }

    std::string worldName;
    std::cout << "Enter name of world to delete: ";
    std::getline(std::cin, worldName);
    if (worldName.empty()) {
        return MenuOptionResult {
            .success = false,
            .msg = "World name cannot be empty"
        };
    }
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
        .msg = fmt::format("Deleted world {}", worldName)
    };
}

MenuOptionResult load_world() {
    // list worlds available
    std::size_t worldCount = 0;
    for (const auto& entry : std::filesystem::directory_iterator("worlds")) {
        if (entry.is_directory()) {
            worldCount++;
            std::cout << "* " << entry.path().filename().string() << ": " << format_bytes(get_size_of_directory(entry)) << std::endl;
        }
    }
    
    // if the user has no worlds, return error
    if (worldCount == 0) {
        return MenuOptionResult {
            .success = false,
            .msg = "You have no worlds to load!"
        };
    }
    
    // get user world choice
    std::string worldName;
    std::cout << "Enter world name: ";
    std::getline(std::cin, worldName);
    if (worldName.empty()) {
        return MenuOptionResult {
            .success = false,
            .msg = "World name cannot be empty"
        };
    }

    std::string worldDirectory = fmt::format("worlds/{}", worldName);
    // if its not a directory, it cannot be a world so return error
    if (!std::filesystem::is_directory(worldDirectory)) {
        return MenuOptionResult{
            .success = false,
            .msg = fmt::format("World {} does not exist", worldName)
        };
    }

    std::cout << GREEN << fmt::format("Started playing world {}", worldName) << RESET << std::endl;
    try {
        Game game;
        game.Run(worldDirectory);
    } catch (const WorldCorruptionException& e) {
        return MenuOptionResult{
            .success = false,
            .msg = e.what()
        };
    }

    return MenuOptionResult{
        .success = true,
        .msg = fmt::format("Finished playing world {}", worldName)
    };
}

void run() {
    Log::GetLogger();
    SoundEngine::PreloadGameSounds();
    std::filesystem::create_directory("worlds");

    MenuOptionResult result{};
    char choice;
    std::cout << R"(  ____            __ _      
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
        std::cout << "Enter your choice: ";
        std::string input;
        std::getline(std::cin, input);
        choice = static_cast<char>(std::tolower(input[0]));

        switch (choice) {
            case 'c': {
                result = create_world();
                break;
            }
            case 'l': {
                result = load_world();    
                break;
            }
            case 'd': {
                result = delete_world();
                break;
            }
            case 'q': {
                break;
            }
            default: {
                result = MenuOptionResult{
                    .success = false,
                    .msg = "Invalid choice"
                };
                break;
            }
        }
        if (!result.msg.empty()) {
            std::cout << (result.success ? GREEN : RED) << result.msg << RESET << std::endl;
        }
    } while (choice != 'q');

}

int main() {
    try{
        run();
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
