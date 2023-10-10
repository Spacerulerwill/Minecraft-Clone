/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/Block.hpp>
#include <util/Log.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

namespace engine {
    float crossVertices[84] = {
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    BlockDataStruct BlockData[NUM_BLOCKS];
    BlockModelStruct BlockModelData[NUM_MODELS] = {
        {}, // Cube has no custom model for obvious reasons,
        {&crossVertices[0], &crossVertices[84]}
    };
}

void engine::InitBlocks() {
	//Load block data from blocks.yml
	YAML::Node blocks_yml = YAML::LoadFile("res/blocks.yml");

	int face_index = 0;
	int block_count = 0;
	for (YAML::const_iterator it = blocks_yml.begin(); it != blocks_yml.end(); ++it) {
		BlockDataStruct blockData{};

		blockData.opaque = it->second["opaque"].as<bool>();
		blockData.material = it->second["material"].as<MaterialInt>();

		uint8_t uniqueFacesCount = it->second["unique_faces"].as<uint8_t>();
		blockData.unique_faces = uniqueFacesCount;

		switch (uniqueFacesCount) {
		case 1: {
			TextureInt face = face_index;
			blockData.top_face = face;
			blockData.bottom_face = face;
			blockData.left_face = face;
			blockData.right_face = face;
			blockData.front_face = face;
			blockData.back_face = face;
			break;
		}
		case 2: {
			TextureInt vertical_face = face_index;
			TextureInt side_face = face_index + 1;
			blockData.top_face = vertical_face;
			blockData.bottom_face = vertical_face;
			blockData.left_face = side_face;
			blockData.right_face = side_face;
			blockData.front_face = side_face;
			blockData.back_face = side_face;
			break;
		}
		case 3: {
			blockData.top_face = face_index;
			blockData.bottom_face = face_index + 1;
			TextureInt side = face_index + 2;
			blockData.left_face = side;
			blockData.right_face = side;
			blockData.front_face = side;
			blockData.back_face = side;
			break;
		}
		case 6: {
			blockData.top_face = face_index;
			blockData.bottom_face = face_index + 1;
			blockData.left_face = face_index + 2;
			blockData.right_face = face_index + 3;
			blockData.front_face = face_index + 4;
			blockData.back_face = face_index + 5;
			break;
		}
		}
		blockData.model = it->second["model"].as<ModelInt>();
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