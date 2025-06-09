#pragma once

#include <cmath>
#include <iostream>
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
	std::map<std::tuple<int32_t, int32_t>, std::vector<BlockType>> data;
	unsigned int chunkLength;
	unsigned int vertexFloatCount;

	Texturer texturer;

	uint32_t imageWidth;
	uint32_t imageHeight;

	void getFreeVertices(std::vector<float>& meshPart, unsigned int index, std::tuple<int32_t, int32_t> chunkCoords, BlockType blockType);

	bool getFreeFace(uint32_t index, std::tuple<int32_t, int32_t> chunkCoords, CubeFace face);

	int intPow(int base, int exp);
public:
	Chunk(const std::map<std::tuple<int32_t, int32_t>, std::vector<BlockType>>& data, unsigned int chunkLength, unsigned int vertexFloatCount, uint32_t imageWidth, uint32_t imageHeight);

	void generateMesh(std::vector<float>& mesh, std::tuple<int32_t, int32_t>);
};