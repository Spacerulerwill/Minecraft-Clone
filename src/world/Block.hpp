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
#include <type_traits>

using TextureID = uint8_t;
using SoundID = uint8_t;
using ModelID = uint8_t;

enum class BlockType : uint8_t {
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
    SNOW,
    NUM_BLOCKS
};

using BlockTypeID = std::underlying_type<BlockType>::type;

class Block {
private:
    uint16_t data{};
public:
    Block(BlockType type, uint8_t rotation, bool waterlogged);
    void SetType(BlockType type);
    void SetRotation(uint8_t rotation);
    void SetWaterlogged(bool waterlogged);
    BlockType GetType() const;
    uint8_t GetRotation() const;
    bool IsWaterLogged() const;
    bool operator==(Block block) const;
    bool operator!=(Block block) const;
};

enum class Sound : SoundID {
    AIR,
    GRASS,
    GRAVEL,
    STONE,
    WOOD,
    GLASS,
    SAND,
    SNOW,
    NUM_SOUNDS
};

enum class Model : ModelID {
    CUBE,
    CROSS,
    NUM_MODELS,
};

enum BlockFaces {
    FRONT_FACE,
    BACK_FACE,
    LEFT_FACE,
    RIGHT_FACE,
    TOP_FACE,
    BOTTOM_FACE
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
    bool waterloggable;
};

struct BlockSoundStruct {
    std::vector<std::string> sounds;
};

using BlockModel = std::vector<uint32_t>;

// Global arrays used to fetch certain information by integer ID, using array indexing
extern BlockDataStruct BlockData[static_cast<std::size_t>(BlockType::NUM_BLOCKS)];
extern BlockSoundStruct BlockSounds[static_cast<std::size_t>(Sound::NUM_SOUNDS)];
extern BlockModel BlockModels[static_cast<std::size_t>(Model::NUM_MODELS)];
constexpr unsigned int TEXTURE_SIZE = 16;
constexpr const std::size_t MAX_ANIMATION_FRAMES = 32;
void InitBlocks();
const BlockDataStruct& GetBlockData(BlockType type);

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
