/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <cstdint>

namespace engine {
    // integer sizes for each ID type
    using BlockInt = uint8_t;
    using MaterialInt = uint8_t;
    using TextureInt = uint8_t;
    using ModelInt = uint8_t;

    // enum of block IDs
    enum Block : BlockInt {
        AIR,
        WATER,
        BEDROCK,
        DIRT,
        COBBLE,
        STONE,
        GLASS,
        OAK_PLANKS,
        CHERRY_PLANKS,
        CHERRY_LOG,
        GRASS,
        TNT,
        OAK_LOG,
        OAK_LEAVES,
        ROSE,
        TALL_GRASS,
        PINK_TULIP,
        SAND,
        CLAY,
        GRAVEL,
        NUM_BLOCKS
    };

    // enum of model IDs
    enum BlockModel : uint8_t {
        CUBE,
        CROSS,
        NUM_MODELS
    };

    /*
    Struct that contains the start and end pointers for an array containing
    the vertex information for a block model
    */
    struct BlockModelStruct {
        uint32_t* begin = nullptr;
        uint32_t* end = nullptr;
    };

    // Struct containing the block information for a single block type
    struct BlockDataStruct {
        bool opaque;
        uint8_t unique_faces;
        MaterialInt material;
        TextureInt top_face;
        TextureInt bottom_face;
        TextureInt left_face;
        TextureInt right_face;
        TextureInt front_face;
        TextureInt back_face;
        ModelInt model;
    };

    // Global arrays used to fetch certain information of blocks by using their ID as array index
    extern BlockDataStruct BlockData[NUM_BLOCKS];
    extern BlockModelStruct BlockModelData[NUM_MODELS];

    /*
    Block texture size in pixels.
    NOTE: You cannot change this value without making refactoring changes to the meshing of
    custom model blocks. Just leave it alone please.
    */
    inline constexpr unsigned int TEXTURE_SIZE = 16;

    // The max possible frames for an animated tile.
    inline constexpr unsigned int MAX_ANIMATION_FRAMES = 32;

    /*
    Loads block data from res/blocks.yml and fills the BlockData array
    with BlockDataStruct for each block.
    NOTE: Must be called before trying to access BlockData!
    */
    void InitBlocks();
};

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