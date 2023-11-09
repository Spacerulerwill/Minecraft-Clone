/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef LOG_H
#define LOG_H

#include <spdlog/spdlog.h>
#include <memory>

#define DEBUG_MODE

namespace engine {
	class Log {
	public:
        // Initialise the logger singleton. Must be called before use.
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

		Log(const Log& arg) = delete; // Copy constructor
		Log(const Log&& arg) = delete;  // Move constructor
		Log& operator=(const Log& arg) = delete; // Assignment operator
		Log& operator=(const Log&& arg) = delete; // Move operator
	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};

};
#endif // !LOG_H

// Logging macros
#ifdef DEBUG_MODE
	#define LOG_CRITICAL(...)    engine::Log::GetLogger()->critical(__VA_ARGS__)
	#define LOG_ERROR(...)		engine::Log::GetLogger()->error(__VA_ARGS__)
	#define LOG_WARNING(...)		engine::Log::GetLogger()->warn(__VA_ARGS__)
	#define LOG_INFO(...)		engine::Log::GetLogger()->info(__VA_ARGS__)
	#define LOG_TRACE(...)		engine::Log::GetLogger()->trace(__VA_ARGS__)
#else
	#define LOG_CRITICAL 
	#define LOG_ERROR 
	#define LOG_WARNING 
	#define LOG_INFO 
	#define LOG_TRACE 
#endif // DEBUG_MODE

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