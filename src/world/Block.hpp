/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef BLOCK_H
#define BLOCK_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

using BlockID = uint8_t;
using TextureID = uint8_t;
using SoundID = uint8_t;
using ModelID = uint8_t;

enum Block : BlockID {
    AIR,
    STONE,
    DIRT,
    GRASS,
    BEDROCK,
    GLASS,
    WATER,
    ROSE,
    PINK_TULIP,
    TALL_GRASS,
    SAND,
    SUGARCANE,
    WOOD,
    SAKURA_LEAVES,
    NUM_BLOCKS
};

enum class Sound : SoundID {
    AIR,
    GRASS,
    GRAVEL,
    STONE,
    WOOD,
    GLASS,
    SAND,
    NUM_SOUNDS
};

enum class Model : ModelID {
    CUBE,
    CROSS,
    NUM_MODELS,
};

enum BlockFaces {
    TOP_FACE,
    BOTTOM_FACE,
    RIGHT_FACE,
    LEFT_FACE,
    BACK_FACE,
    FRONT_FACE
};

struct BlockDataStruct {
    uint8_t unique_faces;
    std::array<TextureID, 6> faces;
    SoundID breakSoundID;
    SoundID placeSoundID;
    ModelID modelID;
    bool opaque;
    bool collision;
    bool canInteractThrough;
};

struct BlockSoundStruct {
    std::vector<std::string> sounds;
};

using BlockModel = std::vector<uint32_t>;

// Global arrays used to fetch certain information by integer ID, using array indexing
extern BlockDataStruct BlockData[NUM_BLOCKS];
extern BlockSoundStruct BlockSounds[static_cast<std::size_t>(Sound::NUM_SOUNDS)];
extern BlockModel BlockModels[static_cast<std::size_t>(Model::NUM_MODELS)];
constexpr unsigned int TEXTURE_SIZE = 16;
constexpr const std::size_t MAX_ANIMATION_FRAMES = 32;
void InitBlocks();

#endif // !BLOCK_H

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
