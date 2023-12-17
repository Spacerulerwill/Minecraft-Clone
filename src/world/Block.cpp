/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/Block.hpp>
#include <yaml-cpp/yaml.h>

BlockDataStruct BlockData[NUM_BLOCKS] = {};

void InitBlocks() {
    //Load block data from blocks.yml
    YAML::Node blocks_yml = YAML::LoadFile("res/blocks.yml");

    int face_index = 0;
    int block_count = 0;
    for (YAML::const_iterator it = blocks_yml.begin(); it != blocks_yml.end(); ++it) {
        BlockDataStruct blockData{};

        uint8_t uniqueFacesCount = it->second["unique_faces"].as<uint8_t>();
        blockData.unique_faces = uniqueFacesCount;

        switch (uniqueFacesCount) {
        case 1: {
            TextureID face = face_index;
            for (int i = 0; i < 6; i++) {
                blockData.faces[i] = face;
            }
            break;
        }
        case 2: {
            TextureID vertical_face = face_index;
            TextureID side_face = face_index + 1;
            blockData.faces[TOP_FACE] = vertical_face;
            blockData.faces[BOTTOM_FACE] = vertical_face;
            blockData.faces[LEFT_FACE] = side_face;
            blockData.faces[RIGHT_FACE] = side_face;
            blockData.faces[FRONT_FACE] = side_face;
            blockData.faces[BACK_FACE] = side_face;
            break;
        }
        case 3: {
            TextureID top_face = face_index;
            TextureID bottom_face = face_index + 1;
            TextureID side_face = face_index + 2;

            blockData.faces[TOP_FACE] = top_face;
            blockData.faces[BOTTOM_FACE] = bottom_face;
            blockData.faces[LEFT_FACE] = side_face;
            blockData.faces[RIGHT_FACE] = side_face;
            blockData.faces[FRONT_FACE] = side_face;
            blockData.faces[BACK_FACE] = side_face;
            break;
        }
        case 6: {
            for (int i = 0; i < 6; i++) {
                blockData.faces[i] = face_index + i;
            }
            break;
        }
        }
        BlockData[block_count] = blockData;
        face_index += uniqueFacesCount;
        block_count++;
    }
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
