#include <util/Log.hpp>
#include <world/Block.hpp>
#include <filesystem>
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

void engine::BlockHandler::InitBlocks() {
	// Load block sounds
	for (const auto& file : std::filesystem::directory_iterator("res/sound/block")) {
		auto char_path = file.path().string();
		LOG_TRACE(char_path);
	}

	// Load materials from materials.yml
	YAML::Node materials_yml = YAML::LoadFile("res/materials.yml");
	int index = 0;
	for (YAML::const_iterator it = materials_yml.begin(); it != materials_yml.end(); ++it, index++) {
		std::string materialName = it->first.as<std::string>();
		
		std::string breakType = it->second["break"].as<std::string>();
		int breakCount = it->second["break_count"].as<int>();
		std::vector<std::string> breakSounds;

		std::string placeType = it->second["place"].as<std::string>();
		int placeCount = it->second["place_count"].as<int>();
		std::vector<std::string> placeSounds;

		for (int i = 0; i < breakCount; i++) {
			breakSounds.push_back(fmt::format("res/sound/block/{}{}.ogg", breakType, i+1));
		}

		for (int i = 0; i < placeCount; i++) {
			placeSounds.push_back(fmt::format("res/sound/block/{}{}.ogg", placeType, i+1));
		}

		MaterialData[index] = MaterialDataStruct{
			breakSounds,
			placeSounds
		};
	}

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