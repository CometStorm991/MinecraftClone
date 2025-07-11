#pragma once

#include <cmath>
#include <iostream>
#include <random>
#include <tuple>
#include <map>
#include <vector>

#include "BlockType.hpp"
#include "Cube.hpp"
#include "CubeFace.hpp"
#include "LockedElement.hpp"
#include "PerformanceTimer.hpp"
#include "Texturer.hpp"
#include "ThreadPool.hpp"

class Chunk
{
private:
	std::vector<BlockType>& chunkData;
	uint32_t chunkLength;
	uint32_t chunkGenLength;
	std::tuple<uint32_t, uint32_t, uint32_t> chunkCounts;

	std::vector<float>& mesh;
	uint32_t vertexFloatCount;

	int32_t minGenerationY = 0;
	int32_t maxGenerationY = 63;

	Texturer texturer;

	uint32_t imageWidth;
	uint32_t imageHeight;

	void placeBlockInChunk(const std::tuple<int32_t, int32_t, int32_t>& blockCoords, BlockType blockType);

	bool getTerrainExists(const std::tuple<int32_t, int32_t, int32_t>& worldCoords);
	BlockType getTerrain(const std::tuple<int32_t, int32_t, int32_t>& worldCoords);

	void getFreeVertices(std::vector<float>& meshPart, const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, const std::tuple<int32_t, int32_t, int32_t>& block, BlockType blockType);
	bool getFreeFace(const std::tuple<int32_t, int32_t, int32_t>& blockCoords, CubeFace face);
	bool getBlockExists(const std::tuple<int32_t, int32_t, int32_t>& worldCoords);

	void chunkToWorldCoords(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, const std::tuple<int32_t, int32_t, int32_t>& blockCoords, std::tuple<int32_t, int32_t, int32_t>& worldCoords);
	int32_t chunkToWorldCoord(int32_t chunkCoord, uint32_t blockCoord);
	void worldToChunkCoords(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, std::tuple<int32_t, int32_t, int32_t>& chunkCoords, std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords);
	std::tuple<int32_t, uint32_t> worldToChunkCoord(int32_t worldCoord);

	int intPow(int base, int exp);
public:
	Chunk(std::vector<BlockType>& chunkData, std::vector<float>& mesh, uint32_t chunkLength, uint32_t vertexFloatCount, uint32_t imageWidth, uint32_t imageHeight);

	void generateBlocks(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords);

	void generateMesh(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords);
};