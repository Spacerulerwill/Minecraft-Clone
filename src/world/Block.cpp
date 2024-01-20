/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/Block.hpp>
#include <yaml-cpp/yaml.h>
#include <format>
#include <util/Log.hpp>

BlockDataStruct BlockData[NUM_BLOCKS] = {};
BlockSoundStruct BlockSounds[static_cast<std::size_t>(Sound::NUM_SOUNDS)] = {};
BlockModel BlockModels[static_cast<std::size_t>(Model::NUM_MODELS)] = {};

void LoadBlockData() {
    //Load block data from blocks.yml
    YAML::Node blocks_yml = YAML::LoadFile("data/blocks/blocks.yml");

    int face_index = 0;
    int block_count = 0;
    for (YAML::const_iterator it = blocks_yml.begin(); it != blocks_yml.end(); ++it) {
        BlockDataStruct blockData{};

        uint8_t uniqueFacesCount = it->second["unique_faces"].as<uint8_t>();
        blockData.unique_faces = uniqueFacesCount;
        blockData.breakSoundID = it->second["break_sound_id"].as<SoundID>();
        blockData.placeSoundID = it->second["place_sound_id"].as<SoundID>();
        blockData.modelID = it->second["model_id"].as<ModelID>();
        blockData.opaque = it->second["opaque"].as<bool>();
        blockData.collision = it->second["collision"].as<bool>();
        blockData.canInteractThrough = it->second["can_interact_through"].as<bool>();

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

void LoadBlockSoundData() {
    // Load sounds
    YAML::Node sounds_yml = YAML::LoadFile("data/blocks/sounds.yml");

    std::size_t index = 0;
    for (YAML::const_iterator it = sounds_yml.begin(); it != sounds_yml.end(); ++it) {
        BlockSoundStruct blockSound{};
        std::string soundName = it->first.as<std::string>();
        std::size_t count = it->second.as<std::size_t>();
        std::vector<std::string> paths;
        for (std::size_t i = 0; i < count; i++) {
            paths.emplace_back(std::format("sound/block/{}{}.ogg", soundName, i + 1));
        }
        blockSound.sounds = std::move(paths);
        BlockSounds[index] = std::move(blockSound);
        index++;
    }
}

void LoadBlockModels() {
    BlockModels[static_cast<std::size_t>(Model::CUBE)] = {};
    BlockModels[static_cast<std::size_t>(Model::CROSS)] = {
        0, 0, 0, 0, 16, 0, 0,
        16, 0, 16, 16, 16, 0, 0,
        16, 16, 16, 16, 0, 0, 0,

        0, 0, 0, 0, 16, 0, 0,
        16, 16, 16, 16, 0, 0, 0,
        0, 16, 0, 0, 0, 0, 0,

        0, 0, 16, 16, 16, 0, 0,
        16, 0, 0, 0, 16, 0, 0,
        16, 16, 0, 0, 0, 0, 0,

        0, 0, 16, 16, 16, 0, 0,
        16, 16, 0, 0, 0, 0, 0,
        0, 16, 16, 16, 0, 0, 0
    };
}

void InitBlocks() {
    LoadBlockData();
    LoadBlockSoundData();
    LoadBlockModels();
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
