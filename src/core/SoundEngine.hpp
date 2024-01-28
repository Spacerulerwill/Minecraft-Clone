/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef SOUND_ENGINE_H
#define SOUND_ENGINE_H

#include <irrKlang/irrKlang.h>
#include <stdexcept>
#include <util/Log.hpp>
#include <memory>
#include <string>

struct irrKlangEngineWrapper {
    irrklang::ISoundEngine* engine = nullptr;

    irrKlangEngineWrapper() : engine(irrklang::createIrrKlangDevice()) {
        LOG_INFO("Initialising sound engine!");
        if (!engine) {
            throw std::runtime_error("Failed to initialise irrKlang sound engine!");
        }
    }

    ~irrKlangEngineWrapper() {
        LOG_INFO("Destroying sound engine!");
        engine->drop();
    }
};

class SoundEngine {
private:
    SoundEngine() = default;
    ~SoundEngine() = default;
public:
    static void PreloadGameSounds();
    inline static irrklang::ISoundEngine* GetEngine() {
        static irrKlangEngineWrapper wrapper;
        return wrapper.engine;
    };
    SoundEngine(const SoundEngine& arg) = delete;
    SoundEngine(const SoundEngine&& arg) = delete;
    SoundEngine& operator=(const SoundEngine& arg) = delete;
    SoundEngine& operator=(const SoundEngine&& arg) = delete;
};

struct ScopedLoopingSound {
	irrklang::ISound* sound = nullptr;
	ScopedLoopingSound(const char* dir) {
		sound = SoundEngine::GetEngine()->play2D(dir, true, false, true);
	}
	~ScopedLoopingSound() {
		sound->stop();
		sound->drop();
	}
};

#endif // !SOUND_ENGINE_H

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
