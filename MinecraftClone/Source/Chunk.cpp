#include "Chunk.hpp"

Chunk::Chunk()
{

}

Chunk::Chunk(const std::vector<bool>& data, unsigned int chunkLength, unsigned int vertexFloatCount)
	: data(data), chunkLength(chunkLength), vertexFloatCount(vertexFloatCount)
{
	
}

void Chunk::getFreeVertices(std::vector<float>& meshPart, unsigned int index)
{
	meshPart.clear();

	// Get left
	if (getFreeLeft(index))
	{
		std::vector<float> left;
		Cube::generatePNTTyLeft(left);
		meshPart.insert(meshPart.end(), left.begin(), left.end());
	}

	// Get right
	if (getFreeRight(index))
	{
		std::vector<float> right;
		Cube::generatePNTTyRight(right);
		meshPart.insert(meshPart.end(), right.begin(), right.end());
	}

	// Get bottom
	if (getFreeBottom(index))
	{
		std::vector<float> bottom;
		Cube::generatePNTTyBottom(bottom);
		meshPart.insert(meshPart.end(), bottom.begin(), bottom.end());
	}

	// Get top
	if (getFreeTop(index))
	{
		std::vector<float> top;
		Cube::generatePNTTyTop(top);
		meshPart.insert(meshPart.end(), top.begin(), top.end());
	}

	// Get back
	if (getFreeBack(index))
	{
		std::vector<float> back;
		Cube::generatePNTTyBack(back);
		meshPart.insert(meshPart.end(), back.begin(), back.end());
	}

	// Get front
	if (getFreeFront(index))
	{
		std::vector<float> front;
		Cube::generatePNTTyFront(front);
		meshPart.insert(meshPart.end(), front.begin(), front.end());
	}

	uint32_t x = (index % intPow(chunkLength, 1)) / intPow(chunkLength, 0);
	uint32_t y = (index % intPow(chunkLength, 2)) / intPow(chunkLength, 1);
	uint32_t z = (index % intPow(chunkLength, 3)) / intPow(chunkLength, 2);

	for (unsigned int i = 0; i < meshPart.size() / vertexFloatCount; i++)
	{
		meshPart[i * vertexFloatCount + 0] += x;
		meshPart[i * vertexFloatCount + 1] += y;
		meshPart[i * vertexFloatCount + 2] += z;

		meshPart[i * vertexFloatCount + 8] = data.at(index);
	}
}

bool Chunk::getFreeLeft(unsigned int index)
{
	if ((index % intPow(chunkLength, 1)) / intPow(chunkLength, 0) == 0)
	{
		return true;
	}
	
	return !data.at(index - intPow(chunkLength, 0));
}

bool Chunk::getFreeRight(unsigned int index)
{
	if ((index % intPow(chunkLength, 1)) / intPow(chunkLength, 0) == chunkLength - 1)
	{
		return true;
	}

	return !data.at(index + intPow(chunkLength, 0));
}

bool Chunk::getFreeBottom(unsigned int index)
{
	if ((index % intPow(chunkLength, 2)) / intPow(chunkLength, 1) == 0)
	{
		return true;
	}

	return !data.at(index - intPow(chunkLength, 1));
}

bool Chunk::getFreeTop(unsigned int index)
{
	if ((index % intPow(chunkLength, 2)) / intPow(chunkLength, 1) == chunkLength - 1)
	{
		return true;
	}

	return !data.at(index + intPow(chunkLength, 1));
}

bool Chunk::getFreeBack(unsigned int index)
{
	if ((index % intPow(chunkLength, 3)) / intPow(chunkLength, 2) == 0)
	{
		return true;
	}
	
	return !data.at(index - intPow(chunkLength, 2));
}

bool Chunk::getFreeFront(unsigned int index)
{
	if ((index % intPow(chunkLength, 3)) / intPow(chunkLength, 2) == chunkLength - 1)
	{
		return true;
	}

	return !data.at(index + intPow(chunkLength, 2));
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
		if (!data.at(i))
		{
			continue;
		}

		std::vector<float> meshPart;
		getFreeVertices(meshPart, i);
		
		mesh.insert(mesh.end(), meshPart.begin(), meshPart.end());
	}
}