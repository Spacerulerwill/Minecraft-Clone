/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GLFW/glfw3.h>
#include <core/Game.hpp>
#include <util/Log.hpp>
#include <core/SoundEngine.hpp>
#include <string>
#include <iostream>
#include <limits>

int main() {
    Log::GetLogger();
	SoundEngine::PreloadGameSounds();
	
    try {
		char choice;
		do {
			std::cout << R"(
  ____            __ _      
 / ___|_ __ __ _ / _| |_  _     _   
| |   | '__/ _` | |_| __|| |_ _| |_ 
| |___| | | (_| |  _| ||_   _|_   _|
 \____|_|  \__,_|_|  \__||_|   |_|  

Choose an option:
C - create world
L - load world
D - delete world
Q - quit
Enter your choice: )";

			choice = std::tolower(std::cin.get());
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cin.clear();

			switch (choice) {
				case 'l': {
					Game game;
					game.Run();
					break;
				}
				case 'q': {
					break;
				}
				default: {
					std::cout << "Invalid input!" << std::endl;
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
