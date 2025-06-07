#pragma once

#include <cmath>
#include <iostream>
#include <vector>

#include "BlockType.hpp"
#include "Cube.hpp"
#include "CubeFace.hpp"
#include "Texturer.hpp"

class Chunk
{
private:
	std::vector<BlockType> data;
	unsigned int chunkLength;
	unsigned int vertexFloatCount;

	Texturer texturer;

	uint32_t imageWidth;
	uint32_t imageHeight;

	void getFreeVertices(std::vector<float>& meshPart, unsigned int index, BlockType blockType);

	bool getFreeLeft(unsigned int index);
	bool getFreeRight(unsigned int index);
	bool getFreeBottom(unsigned int index);
	bool getFreeTop(unsigned int index);
	bool getFreeBack(unsigned int index);
	bool getFreeFront(unsigned int index);

	int intPow(int base, int exp);
public:
	Chunk(const std::vector<BlockType>& data, unsigned int chunkLength, unsigned int vertexFloatCount, uint32_t imageWidth, uint32_t imageHeight);

	void generateMesh(std::vector<float>& mesh);
};