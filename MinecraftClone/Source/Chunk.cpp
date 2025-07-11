#include "Chunk.hpp"

Chunk::Chunk(std::vector<BlockType>& chunkData, std::vector<float>& mesh, uint32_t chunkLength, uint32_t vertexFloatCount, uint32_t imageWidth, uint32_t imageHeight)
	: chunkData(chunkData), mesh(mesh), chunkLength(chunkLength), chunkGenLength(chunkLength + 2), vertexFloatCount(vertexFloatCount), imageWidth(imageWidth), imageHeight(imageHeight),
	texturer(Texturer(imageWidth, imageHeight))
{
}

void Chunk::placeBlockInChunk(const std::tuple<int32_t, int32_t, int32_t>& blockCoords, BlockType blockType)
{
	int32_t blockX = std::get<0>(blockCoords);
	int32_t blockY = std::get<1>(blockCoords);
	int32_t blockZ = std::get<2>(blockCoords);

	int32_t blockGenX = blockX + 1;
	int32_t blockGenY = blockY + 1;
	int32_t blockGenZ = blockZ + 1;

	chunkData.at(blockGenZ * intPow(chunkGenLength, 2) + blockGenY * intPow(chunkGenLength, 1) + blockGenX * intPow(chunkGenLength, 0)) = blockType;
}

void Chunk::generateBlocks(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords)
{
	int32_t chunkX = std::get<0>(chunkCoords);
	int32_t chunkY = std::get<1>(chunkCoords);
	int32_t chunkZ = std::get<2>(chunkCoords);

	for (uint32_t i = 0; i < intPow(chunkGenLength, 3); i++)
	{
		int32_t blockX = (i % intPow(chunkGenLength, 1)) / intPow(chunkGenLength, 0) - 1;
		int32_t blockY = (i % intPow(chunkGenLength, 2)) / intPow(chunkGenLength, 1) - 1;
		int32_t blockZ = (i % intPow(chunkGenLength, 3)) / intPow(chunkGenLength, 2) - 1;
		std::tuple<int32_t, int32_t, int32_t> blockCoords = std::make_tuple(blockX, blockY, blockZ);

		std::tuple<int32_t, int32_t, int32_t> worldCoords;
		chunkToWorldCoords(chunkCoords, blockCoords, worldCoords);

		BlockType block = getTerrain(worldCoords);

		placeBlockInChunk(blockCoords, block);
	}
}

bool Chunk::getTerrainExists(const std::tuple<int32_t, int32_t, int32_t>& worldCoords)
{
	int32_t worldX = std::get<0>(worldCoords);
	int32_t worldY = std::get<1>(worldCoords);
	int32_t worldZ = std::get<2>(worldCoords);

	if (worldY < minGenerationY || worldY > maxGenerationY)
	{
		return false;
	}

	int32_t maxY = 8.0f * (std::sin(worldX / 8.0f) + std::sin(worldZ / 8.0f) + 2.0f);
	maxY = std::min(maxY, maxGenerationY);
	maxY = std::max(maxY, minGenerationY);

	return worldY <= maxY;
}

BlockType Chunk::getTerrain(const std::tuple<int32_t, int32_t, int32_t>& worldCoords)
{
	if (!getTerrainExists(worldCoords))
	{
		return BlockType::Air;
	}

	int32_t worldY = std::get<1>(worldCoords);
	if (worldY < 8)
	{
		return BlockType::Sand;
	}
	else
	{
		return BlockType::Grass;
	}
}

void Chunk::getFreeVertices(std::vector<float>& meshPart, const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, const std::tuple<int32_t, int32_t, int32_t>& blockCoords, BlockType blockType)
{
	meshPart.clear();

	int32_t blockX = std::get<0>(blockCoords);
	int32_t blockY = std::get<1>(blockCoords);
	int32_t blockZ = std::get<2>(blockCoords);

	// Get left
	if (getFreeFace(blockCoords, CubeFace::Left))
	{
		std::vector<float> left;
		texturer.generateFace(left, CubeFace::Left, blockType);
		meshPart.insert(meshPart.end(), left.begin(), left.end());
	}

	// Get right
	if (getFreeFace(blockCoords, CubeFace::Right))
	{
		std::vector<float> right;
		texturer.generateFace(right, CubeFace::Right, blockType);
		meshPart.insert(meshPart.end(), right.begin(), right.end());
	}

	// Get bottom
	if (getFreeFace(blockCoords, CubeFace::Bottom))
	{
		std::vector<float> bottom;
		texturer.generateFace(bottom, CubeFace::Bottom, blockType);
		meshPart.insert(meshPart.end(), bottom.begin(), bottom.end());
	}

	// Get top
	if (getFreeFace(blockCoords, CubeFace::Top))
	{
		std::vector<float> top;
		texturer.generateFace(top, CubeFace::Top, blockType);
		meshPart.insert(meshPart.end(), top.begin(), top.end());
	}

	// Get back
	if (getFreeFace(blockCoords, CubeFace::Back))
	{
		std::vector<float> back;
		texturer.generateFace(back, CubeFace::Back, blockType);
		meshPart.insert(meshPart.end(), back.begin(), back.end());
	}

	// Get front
	if (getFreeFace(blockCoords, CubeFace::Front))
	{
		std::vector<float> front;
		texturer.generateFace(front, CubeFace::Front, blockType);
		meshPart.insert(meshPart.end(), front.begin(), front.end());
	}

	std::tuple<int32_t, int32_t, int32_t> worldCoords;
	chunkToWorldCoords(chunkCoords, blockCoords, worldCoords);
	int32_t worldX = std::get<0>(worldCoords);
	int32_t worldY = std::get<1>(worldCoords);
	int32_t worldZ = std::get<2>(worldCoords);

	// Offset cube model by location within chunk
	for (unsigned int i = 0; i < meshPart.size() / vertexFloatCount; i++)
	{
		meshPart.at(i * vertexFloatCount + 0) += worldX;
		meshPart.at(i * vertexFloatCount + 1) += worldY;
		meshPart.at(i * vertexFloatCount + 2) += worldZ;
	}
}

bool Chunk::getFreeFace(const std::tuple<int32_t, int32_t, int32_t>& blockCoords, CubeFace face)
{
	int32_t blockX = std::get<0>(blockCoords);
	int32_t blockY = std::get<1>(blockCoords);
	int32_t blockZ = std::get<2>(blockCoords);

	switch (face)
	{
	case CubeFace::Left:
		return !getBlockExists(std::make_tuple(blockX - 1, blockY, blockZ));
	case CubeFace::Right:
		return !getBlockExists(std::make_tuple(blockX + 1, blockY, blockZ));
	case CubeFace::Bottom:
		return !getBlockExists(std::make_tuple(blockX, blockY - 1, blockZ));
	case CubeFace::Top:
		return !getBlockExists(std::make_tuple(blockX, blockY + 1, blockZ));
	case CubeFace::Back:
		return !getBlockExists(std::make_tuple(blockX, blockY, blockZ - 1));
	case CubeFace::Front:
		return !getBlockExists(std::make_tuple(blockX, blockY, blockZ + 1));
	}

	return false;
}

void Chunk::chunkToWorldCoords(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, const std::tuple<int32_t, int32_t, int32_t>& blockCoords, std::tuple<int32_t, int32_t, int32_t>& worldCoords)
{
	int32_t chunkX = std::get<0>(chunkCoords);
	int32_t chunkY = std::get<1>(chunkCoords);
	int32_t chunkZ = std::get<2>(chunkCoords);

	int32_t blockX = std::get<0>(blockCoords);
	int32_t blockY = std::get<1>(blockCoords);
	int32_t blockZ = std::get<2>(blockCoords);

	std::get<0>(worldCoords) = chunkX * chunkLength + blockX;
	std::get<1>(worldCoords) = chunkY * chunkLength + blockY;
	std::get<2>(worldCoords) = chunkZ * chunkLength + blockZ;
}

int32_t Chunk::chunkToWorldCoord(int32_t chunkCoord, uint32_t blockCoord)
{
	return chunkCoord * chunkLength + blockCoord;
}

void Chunk::worldToChunkCoords(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, std::tuple<int32_t, int32_t, int32_t>& chunkCoords, std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords)
{
	int32_t worldX = std::get<0>(worldCoords);
	int32_t worldY = std::get<1>(worldCoords);
	int32_t worldZ = std::get<2>(worldCoords);

	std::get<0>(chunkCoords) = worldX / chunkLength;
	std::get<1>(chunkCoords) = worldY / chunkLength;
	std::get<2>(chunkCoords) = worldZ / chunkLength;

	std::get<0>(blockCoords) = (worldX % chunkLength + chunkLength) % chunkLength;
	std::get<1>(blockCoords) = (worldY % chunkLength + chunkLength) % chunkLength;
	std::get<2>(blockCoords) = (worldZ % chunkLength + chunkLength) % chunkLength;
}

std::tuple<int32_t, uint32_t> Chunk::worldToChunkCoord(int32_t worldCoord)
{
	int32_t chunkCoord = worldCoord / chunkLength;
	uint32_t blockCoord = (worldCoord % chunkLength + chunkLength) % chunkLength;

	return std::make_tuple(chunkCoord, blockCoord);
}

bool Chunk::getBlockExists(const std::tuple<int32_t, int32_t, int32_t>& blockCoords)
{
	int32_t blockX = std::get<0>(blockCoords);
	int32_t blockY = std::get<1>(blockCoords);
	int32_t blockZ = std::get<2>(blockCoords);

	int32_t blockGenX = blockX + 1;
	int32_t blockGenY = blockY + 1;
	int32_t blockGenZ = blockZ + 1;

	return chunkData.at(blockGenZ * intPow(chunkGenLength, 2) + blockGenY * intPow(chunkGenLength, 1) + blockGenX * intPow(chunkGenLength, 0)) != BlockType::Air;
}

int Chunk::intPow(int base, int exp)
{
	return static_cast<int>(std::powf(base, exp));
}

void Chunk::generateMesh(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords)
{
	mesh.clear();
	for (unsigned int i = 0; i < chunkData.size(); i++)
	{
		BlockType blockType = chunkData.at(i);

		if (blockType == BlockType::Air)
		{
			continue;
		}

		int32_t blockX = (i % intPow(chunkGenLength, 1)) / intPow(chunkGenLength, 0) - 1;
		int32_t blockY = (i % intPow(chunkGenLength, 2)) / intPow(chunkGenLength, 1) - 1;
		int32_t blockZ = (i % intPow(chunkGenLength, 3)) / intPow(chunkGenLength, 2) - 1;

		if (blockX < 0 || blockX > chunkLength - 1)
		{
			continue;
		}
		if (blockY < 0 || blockY > chunkLength - 1)
		{
			continue;
		}
		if (blockZ < 0 || blockZ > chunkLength - 1)
		{
			continue;
		}

		std::tuple<uint32_t, uint32_t, uint32_t> blockCoords = std::make_tuple(blockX, blockY, blockZ);

		std::vector<float> meshPart;
		getFreeVertices(meshPart, chunkCoords, blockCoords, blockType);
		
		mesh.insert(mesh.end(), meshPart.begin(), meshPart.end());
	}
}