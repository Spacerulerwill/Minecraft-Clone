import os

for root, subdirs, files in os.walk("src"):
    for file in files:
        if file.endswith(".hpp") or file.endswith(".cpp"):
            file_path = os.path.join(root, file)
            with open(file_path, "r") as f:
                filedata = f.read()
            for original, replacement in [
                ("CS_P_MINUS_ONE", "Chunk::SIZE_PADDED_SUB_1"),
                ("CS_P3", "Chunk::SIZE_PADDED_CUBED"),
                ("CS_P2", "Chunk::SIZE_PADDED_SQUARED"),
                ("CS_P", "Chunk::SIZE_PADDED"),
                ("CS_OVER_2", "Chunk::HALF_SIZE"),
                ("CS_MINUS_ONE", "Chunk::SIZE_SUB_1"),
                ("CS", "Chunk::SIZE"),
                ("CHUNK_SIZE_EXP_X2", "Chunk::SIZE_PADDED_SQUARED_LOG_2"),
                ("CHUNK_SIZE_EXP", "Chunk::SIZE_PADDED_LOG_2"),
                ("DEFAULT_CHUNK_STACK_HEIGHT_MINUS_ONE", "Chunkstack::DEFAULT_SIZE_SUB_1"),
                ("DEFAULT_CHUNK_STACK_HEIGHT", "ChunkStack::DEFAULT_SIZE"),
                ("MAX_MINUS_MIN_WORLD_GEN_HEIGHT", "World::MAX_SUB_MIN_GEN_HEIGHT"),
                ("MAX_WORLD_GEN_HEIGHT", "World::MAX_GEN_HEIGHT"),
                ("MIN_WORLD_GEN_HEIGHT", "World::MIN_GEN_HEIGHT"),
                ("WATER_LEVEL", "World::WATER_LEVEL"),
            ]:
                filedata = filedata.replace(original, replacement)
            with open(file_path, "w") as f:
                f.write(filedata)
        
