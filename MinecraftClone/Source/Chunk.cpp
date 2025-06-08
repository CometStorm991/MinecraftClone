#include "Chunk.hpp"

Chunk::Chunk(const std::vector<BlockType>& data, unsigned int chunkLength, unsigned int vertexFloatCount, uint32_t imageWidth, uint32_t imageHeight)
	: data(data), chunkLength(chunkLength), vertexFloatCount(vertexFloatCount), imageWidth(imageWidth), imageHeight(imageHeight),
	texturer(Texturer(imageWidth, imageHeight))
{
	
}

void Chunk::getFreeVertices(std::vector<float>& meshPart, unsigned int index, BlockType blockType)
{
	meshPart.clear();

	// Get left
	if (getFreeFace(index, CubeFace::Left))
	{
		std::vector<float> left;
		texturer.generateFace(left, CubeFace::Left, blockType);
		meshPart.insert(meshPart.end(), left.begin(), left.end());
	}

	// Get right
	if (getFreeFace(index, CubeFace::Right))
	{
		std::vector<float> right;
		texturer.generateFace(right, CubeFace::Right, blockType);
		meshPart.insert(meshPart.end(), right.begin(), right.end());
	}

	// Get bottom
	if (getFreeFace(index, CubeFace::Bottom))
	{
		std::vector<float> bottom;
		texturer.generateFace(bottom, CubeFace::Bottom, blockType);
		meshPart.insert(meshPart.end(), bottom.begin(), bottom.end());
	}

	// Get top
	if (getFreeFace(index, CubeFace::Top))
	{
		std::vector<float> top;
		texturer.generateFace(top, CubeFace::Top, blockType);
		meshPart.insert(meshPart.end(), top.begin(), top.end());
	}

	// Get back
	if (getFreeFace(index, CubeFace::Back))
	{
		std::vector<float> back;
		texturer.generateFace(back, CubeFace::Back, blockType);
		meshPart.insert(meshPart.end(), back.begin(), back.end());
	}

	// Get front
	if (getFreeFace(index, CubeFace::Front))
	{
		std::vector<float> front;
		texturer.generateFace(front, CubeFace::Front, blockType);
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
	}
}

bool Chunk::getFreeFace(uint32_t index, CubeFace face)
{
	switch (face)
	{
	case CubeFace::Left:
		if ((index % intPow(chunkLength, 1)) / intPow(chunkLength, 0) == 0)
		{
			return true;
		}

		return !static_cast<int>(data.at(index - intPow(chunkLength, 0)));
	case CubeFace::Right:
		if ((index % intPow(chunkLength, 1)) / intPow(chunkLength, 0) == chunkLength - 1)
		{
			return true;
		}

		return !static_cast<int>(data.at(index + intPow(chunkLength, 0)));
	case CubeFace::Bottom:
		if ((index % intPow(chunkLength, 2)) / intPow(chunkLength, 1) == 0)
		{
			return true;
		}

		return !static_cast<int>(data.at(index - intPow(chunkLength, 1)));
	case CubeFace::Top:
		if ((index % intPow(chunkLength, 2)) / intPow(chunkLength, 1) == chunkLength - 1)
		{
			return true;
		}

		return !static_cast<int>(data.at(index + intPow(chunkLength, 1)));
	case CubeFace::Back:
		if ((index % intPow(chunkLength, 3)) / intPow(chunkLength, 2) == 0)
		{
			return true;
		}

		return !static_cast<int>(data.at(index - intPow(chunkLength, 2)));
	case CubeFace::Front:
		if ((index % intPow(chunkLength, 3)) / intPow(chunkLength, 2) == chunkLength - 1)
		{
			return true;
		}

		return !static_cast<int>(data.at(index + intPow(chunkLength, 2)));
	}

	return false;
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
		BlockType blockType = data.at(i);

		if (blockType == BlockType::Air)
		{
			continue;
		}

		std::vector<float> meshPart;
		getFreeVertices(meshPart, i, blockType);
		
		mesh.insert(mesh.end(), meshPart.begin(), meshPart.end());
	}
}