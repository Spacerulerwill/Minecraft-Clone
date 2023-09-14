/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <cstdint>
#include <string>

namespace engine {
	typedef uint8_t BlockInt;
	typedef uint8_t MaterialInt;
	typedef uint8_t TextureInt;
	typedef uint8_t ModelInt;

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
		NUM_BLOCKS
	};

	enum Material : MaterialInt {
		MATERIAL_AIR,
		MATERIAL_GRAVEL,
		MATERIAL_STONE,
		MATERIAL_WOOD,
		MATERIAL_GLASS,
		MATERIAL_GRASS,
		NUM_MATERIALS
	};

	struct MaterialDataStruct {
		std::vector<std::string> breakSounds;
		std::vector<std::string> placeSounds;
	};

	enum BlockModel: uint8_t {
		CUBE,
		CROSS,
		NUM_MODELS
	};

	struct BlockModelStruct {
		float* begin = nullptr;
		float* end = nullptr;
	};

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

	extern BlockDataStruct BlockData[NUM_BLOCKS];
	extern MaterialDataStruct MaterialData[NUM_MATERIALS];
	extern BlockModelStruct BlockModelData[NUM_MODELS];

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