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
	std::map<std::tuple<int32_t, int32_t, int32_t>, LockedElement<std::vector<BlockType>>> blocks;
	std::mutex blocksMutex;
	unsigned int chunkLength;
	std::tuple<uint32_t, uint32_t, uint32_t> chunkCounts;
	unsigned int vertexFloatCount;
	int32_t minGenerationY = 0;
	int32_t maxGenerationY = 63;

	Texturer texturer;

	uint32_t imageWidth;
	uint32_t imageHeight;

	void placeBlockInChunk(std::vector<BlockType>& chunkData, std::tuple<uint32_t, uint32_t, uint32_t> blockCoords, BlockType blockType);

	void getFreeVertices(std::vector<float>& meshPart, const std::tuple<int32_t, int32_t, int32_t>& worldCoords, BlockType blockType);

	bool getFreeFace(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, CubeFace face);
	bool getBlockExists(const std::tuple<int32_t, int32_t, int32_t>& worldCoords);

	void chunkToWorldCoords(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, const std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords, std::tuple<int32_t, int32_t, int32_t>& worldCoords);
	int32_t chunkToWorldCoord(int32_t chunkCoord, uint32_t blockCoord);
	void worldToChunkCoords(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, std::tuple<int32_t, int32_t, int32_t>& chunkCoords, std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords);
	std::tuple<int32_t, uint32_t> worldToChunkCoord(int32_t worldCoord);

	int intPow(int base, int exp);
public:
	Chunk(unsigned int chunkLength, const std::tuple<uint32_t, uint32_t, uint32_t>& chunkCounts, unsigned int vertexFloatCount, uint32_t imageWidth, uint32_t imageHeight);

	std::vector<BlockType>& lockChunk(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords);
	void unlockChunk(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords);
	void generateBlocks(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, std::vector<BlockType>& chunkData);
	int32_t getMaxColumnWorldY(int32_t worldX, int32_t worldZ);

	void generateMesh(std::vector<float>& mesh, std::tuple<int32_t, int32_t, int32_t>);
};