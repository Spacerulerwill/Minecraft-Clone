#ifndef RLE_HPP
#define RLE_HPP

#include <vector>
#include <world/Block.hpp>
#include <world/chunk/Chunk.hpp>

std::vector<std::pair<int, BlockType>> RunLengthEncodeBlocks(const std::vector<int>& input) {
    std::vector<std::pair<int, BlockType>> encoded;

    if (input.empty())
        return encoded;

    int current = input[0];
    int count = 1;
    for (size_t i = 1; i < input.size(); ++i) {
        if (input[i] == current) {
            ++count;
        }
        else {
            encoded.push_back(std::make_pair(current, count));
            current = input[i];
            count = 1;
        }
    }
    encoded.push_back(std::make_pair(current, count));
    return encoded;
}

std::vector<BlockType> DecodeRunLengthEncodedBlocks(std::vector<std::pair<int, BlockType>> input) {
    std::vector<BlockType> blocks;
    blocks.reserve(Chunk::SIZE_PADDED_CUBED);
    for (auto& [count, block] : input) {
        for (int i = 0; i < count; i++) {
            blocks.push_back(block);
        }
    }
    return blocks;
}

#endif // !RLE_HPP