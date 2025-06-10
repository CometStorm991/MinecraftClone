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
#include "Texturer.hpp"

class Chunk
{
private:
	std::map<std::tuple<int32_t, int32_t, int32_t>, std::vector<BlockType>> data;
	unsigned int chunkLength;
	uint32_t chunkRadius;
	unsigned int vertexFloatCount;

	Texturer texturer;

	uint32_t imageWidth;
	uint32_t imageHeight;

	void getFreeVertices(std::vector<float>& meshPart, const std::tuple<int32_t, int32_t, int32_t>& worldCoords, BlockType blockType);

	bool getFreeFace(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, CubeFace face);
	bool getBlockExists(const std::tuple<int32_t, int32_t, int32_t>& worldCoords);

	void chunkToWorldCoords(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, const std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords, std::tuple<int32_t, int32_t, int32_t>& worldCoords);
	void worldToChunkCoords(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, std::tuple<int32_t, int32_t, int32_t>& chunkCoords, std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords);

	int intPow(int base, int exp);
public:
	Chunk(const std::map<std::tuple<int32_t, int32_t, int32_t>, std::vector<BlockType>>& data, unsigned int chunkLength, uint32_t chunkRadius, unsigned int vertexFloatCount, uint32_t imageWidth, uint32_t imageHeight);

	void generateBlocks();
	void generateMesh(std::vector<float>& mesh, std::tuple<int32_t, int32_t, int32_t>);
};