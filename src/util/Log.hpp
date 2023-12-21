/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef LOG_H
#define LOG_H

#include <memory>
#include <spdlog/spdlog.h>          

class Log {
public:
    static void Init();
    inline static std::shared_ptr<spdlog::logger>& GetLogger() { return sLogger; }

    Log(const Log& arg) = delete;
    Log(const Log&& arg) = delete;
    Log& operator=(const Log& arg) = delete;
    Log& operator=(const Log&& arg) = delete;
private:
    static std::shared_ptr<spdlog::logger> sLogger;
};

#define LOG_CRITICAL(...)       Log::GetLogger()->critical(__VA_ARGS__)
#define LOG_ERROR(...)		    Log::GetLogger()->error(__VA_ARGS__)
#define LOG_WARNING(...)		Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...)		    Log::GetLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...)	    	Log::GetLogger()->trace(__VA_ARGS__)

#endif // !LOG_H

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