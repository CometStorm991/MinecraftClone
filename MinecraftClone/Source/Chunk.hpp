#pragma once

#include <cmath>
#include <iostream>
#include <vector>

#include "Cube.hpp"

class Chunk
{
private:
	std::vector<bool> data;
	unsigned int chunkSize;

	void getFreeVertices(std::vector<float>& meshPart, unsigned int index);

	bool getFreeLeft(unsigned int index);
	bool getFreeRight(unsigned int index);
	bool getFreeBottom(unsigned int index);
	bool getFreeTop(unsigned int index);
	bool getFreeBack(unsigned int index);
	bool getFreeFront(unsigned int index);

	int intPow(int base, int exp);
public:
	Chunk();
	Chunk(const std::vector<bool>& data, unsigned int chunkSize);

	void generateMesh(std::vector<float>& mesh);
};