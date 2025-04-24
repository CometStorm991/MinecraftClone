#include "Chunk.hpp"

Chunk::Chunk()
{

}

Chunk::Chunk(const std::vector<bool>& data, unsigned int chunkSize)
	: data(data), chunkSize(chunkSize)
{

}

void Chunk::getFreeVertices(std::vector<float>& meshPart, unsigned int index)
{
	meshPart.clear();

	// Get left
	if (getFreeLeft(index))
	{
		std::vector<float> left;
		Cube::generatePNLeft(left);
		meshPart.insert(meshPart.end(), left.begin(), left.end());
	}

	// Get right
	if (getFreeRight(index))
	{
		std::vector<float> right;
		Cube::generatePNRight(right);
		meshPart.insert(meshPart.end(), right.begin(), right.end());
	}

	// Get bottom
	if (getFreeBottom(index))
	{
		std::vector<float> bottom;
		Cube::generatePNBottom(bottom);
		meshPart.insert(meshPart.end(), bottom.begin(), bottom.end());
	}

	// Get top
	if (getFreeTop(index))
	{
		std::vector<float> top;
		Cube::generatePNTop(top);
		meshPart.insert(meshPart.end(), top.begin(), top.end());
	}

	// Get back
	if (getFreeBack(index))
	{
		std::vector<float> back;
		Cube::generatePNBack(back);
		meshPart.insert(meshPart.end(), back.begin(), back.end());
	}

	// Get front
	if (getFreeFront(index))
	{
		std::vector<float> front;
		Cube::generatePNFront(front);
		meshPart.insert(meshPart.end(), front.begin(), front.end());
	}

	uint32_t x = (index % intPow(chunkSize, 1)) / intPow(chunkSize, 0);
	uint32_t y = (index % intPow(chunkSize, 2)) / intPow(chunkSize, 1);
	uint32_t z = (index % intPow(chunkSize, 3)) / intPow(chunkSize, 2);

	for (unsigned int i = 0; i < meshPart.size() / 6; i++)
	{
		meshPart[i * 6 + 0] += x;
		meshPart[i * 6 + 1] += y;
		meshPart[i * 6 + 2] += z;
	}
}

bool Chunk::getFreeLeft(unsigned int index)
{
	if ((index % intPow(chunkSize, 1)) / intPow(chunkSize, 0) == 0)
	{
		return true;
	}
	
	return !data.at(index - intPow(chunkSize, 0));
}

bool Chunk::getFreeRight(unsigned int index)
{
	if ((index % intPow(chunkSize, 1)) / intPow(chunkSize, 0) == chunkSize - 1)
	{
		return true;
	}

	return !data.at(index + intPow(chunkSize, 0));
}

bool Chunk::getFreeBottom(unsigned int index)
{
	if ((index % intPow(chunkSize, 2)) / intPow(chunkSize, 1) == 0)
	{
		return true;
	}

	return !data.at(index - intPow(chunkSize, 1));
}

bool Chunk::getFreeTop(unsigned int index)
{
	if ((index % intPow(chunkSize, 2)) / intPow(chunkSize, 1) == chunkSize - 1)
	{
		return true;
	}

	return !data.at(index + intPow(chunkSize, 1));
}

bool Chunk::getFreeBack(unsigned int index)
{
	if ((index % intPow(chunkSize, 3)) / intPow(chunkSize, 2) == 0)
	{
		return true;
	}
	
	return !data.at(index - intPow(chunkSize, 2));
}

bool Chunk::getFreeFront(unsigned int index)
{
	if ((index % intPow(chunkSize, 3)) / intPow(chunkSize, 2) == chunkSize - 1)
	{
		return true;
	}

	return !data.at(index + intPow(chunkSize, 2));
}

int Chunk::intPow(int base, int exp)
{
	return static_cast<int>(std::powf(base, exp));
}

void Chunk::generateMesh(std::vector<float>& mesh)
{
	mesh.clear();
	for (unsigned int i = 0; i < data.size(); i++)
	{
		std::vector<float> meshPart;
		getFreeVertices(meshPart, i);
		
		mesh.insert(mesh.end(), meshPart.begin(), meshPart.end());
	}
}