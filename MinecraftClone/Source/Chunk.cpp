#include "Chunk.hpp"

Chunk::Chunk(unsigned int chunkLength, const std::tuple<uint32_t, uint32_t, uint32_t>& chunkCounts, unsigned int vertexFloatCount, uint32_t imageWidth, uint32_t imageHeight)
	: chunkLength(chunkLength), chunkCounts(chunkCounts), vertexFloatCount(vertexFloatCount), imageWidth(imageWidth), imageHeight(imageHeight),
	texturer(Texturer(imageWidth, imageHeight))
{
}

void Chunk::placeBlockInChunk(std::vector<BlockType>& chunkData, std::tuple<uint32_t, uint32_t, uint32_t> blockCoords, BlockType blockType)
{
	uint32_t blockX = std::get<0>(blockCoords);
	uint32_t blockY = std::get<1>(blockCoords);
	uint32_t blockZ = std::get<2>(blockCoords);

	chunkData.at(blockZ * intPow(chunkLength, 2) + blockY * intPow(chunkLength, 1) + blockX * intPow(chunkLength, 0)) = blockType;
}

std::vector<BlockType>& Chunk::lockChunk(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords)
{
	if (blocks.find(chunkCoords) == blocks.end()) // not found
	{
		blocks.emplace(chunkCoords, LockedElement<std::vector<BlockType>>());
		std::vector<BlockType>& chunkData = blocks.at(chunkCoords).value;
		chunkData.resize(intPow(chunkLength, 3));
	}

	std::unique_lock<std::mutex> blocksMutexLock(blocksMutex);
	blocks.at(chunkCoords).mutex.lock();
	blocksMutexLock.unlock();
}

void Chunk::unlockChunk(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords)
{
	std::unique_lock<std::mutex> blocksMutexLock(blocksMutex);
	blocks.at(chunkCoords).mutex.unlock();
	blocksMutexLock.unlock();
}

void Chunk::generateBlocks(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, std::vector<BlockType>& chunkData)
{
	int32_t chunkX = std::get<0>(chunkCoords);
	int32_t chunkY = std::get<1>(chunkCoords);
	int32_t chunkZ = std::get<2>(chunkCoords);
	
	for (uint32_t i = 0; i < intPow(chunkLength, 2); i++)
	{
		uint32_t blockX = (i % intPow(chunkLength, 1)) / intPow(chunkLength, 0);
		uint32_t blockZ = (i % intPow(chunkLength, 2)) / intPow(chunkLength, 1);
		std::tuple<uint32_t, uint32_t, uint32_t> blockCoords = std::make_tuple(blockX, 0, blockZ);

		std::tuple<int32_t, int32_t, int32_t> worldCoords;
		chunkToWorldCoords(chunkCoords, blockCoords, worldCoords);
		int32_t worldX = std::get<0>(worldCoords);
		int32_t worldZ = std::get<2>(worldCoords);

		int32_t minColumnWorldY = chunkToWorldCoord(chunkY, 0);
		int32_t maxColumnWorldY = chunkToWorldCoord(chunkY, chunkLength - 1);

		int32_t worldYLimit = getMaxColumnWorldY(worldX, worldZ);

		minColumnWorldY = std::max(minColumnWorldY, minGenerationY);
		maxColumnWorldY = std::min(maxColumnWorldY, worldYLimit);
		
		uint32_t minColumnBlockY = std::get<1>(worldToChunkCoord(minColumnWorldY));
		uint32_t maxColumnBlockY = std::get<1>(worldToChunkCoord(maxColumnWorldY));

		for (uint32_t j = minColumnBlockY; j <= maxColumnBlockY; j++)
		{
			uint32_t blockY = j;
			int32_t worldY = chunkToWorldCoord(chunkY, blockY);
			std::tuple<uint32_t, uint32_t, uint32_t> blockCoords = std::make_tuple(blockX, blockY, worldZ);

			if (worldY < 8)
			{
				placeBlockInChunk(chunkData, blockCoords, BlockType::Sand);
			}
			else
			{
				placeBlockInChunk(chunkData, blockCoords, BlockType::Grass);
			}
		}
	}
}

int32_t Chunk::getMaxColumnWorldY(int32_t worldX, int32_t worldZ)
{
	int32_t maxY = 8.0f * (std::sin(worldX / 8.0f) + std::sin(worldZ / 8.0f) + 2.0f);
	maxY = std::min(maxY, maxGenerationY);
	maxY = std::max(maxY, minGenerationY);
	return maxY;
}

void Chunk::getFreeVertices(std::vector<float>& meshPart, const std::tuple<int32_t, int32_t, int32_t>& worldCoords, BlockType blockType)
{
	meshPart.clear();

	int32_t worldX = std::get<0>(worldCoords);
	int32_t worldY = std::get<1>(worldCoords);
	int32_t worldZ = std::get<2>(worldCoords);

	// Get left
	if (getFreeFace(worldCoords, CubeFace::Left))
	{
		std::vector<float> left;
		texturer.generateFace(left, CubeFace::Left, blockType);
		meshPart.insert(meshPart.end(), left.begin(), left.end());
	}

	// Get right
	if (getFreeFace(worldCoords, CubeFace::Right))
	{
		std::vector<float> right;
		texturer.generateFace(right, CubeFace::Right, blockType);
		meshPart.insert(meshPart.end(), right.begin(), right.end());
	}

	// Get bottom
	if (getFreeFace(worldCoords, CubeFace::Bottom))
	{
		std::vector<float> bottom;
		texturer.generateFace(bottom, CubeFace::Bottom, blockType);
		meshPart.insert(meshPart.end(), bottom.begin(), bottom.end());
	}

	// Get top
	if (getFreeFace(worldCoords, CubeFace::Top))
	{
		std::vector<float> top;
		texturer.generateFace(top, CubeFace::Top, blockType);
		meshPart.insert(meshPart.end(), top.begin(), top.end());
	}

	// Get back
	if (getFreeFace(worldCoords, CubeFace::Back))
	{
		std::vector<float> back;
		texturer.generateFace(back, CubeFace::Back, blockType);
		meshPart.insert(meshPart.end(), back.begin(), back.end());
	}

	// Get front
	if (getFreeFace(worldCoords, CubeFace::Front))
	{
		std::vector<float> front;
		texturer.generateFace(front, CubeFace::Front, blockType);
		meshPart.insert(meshPart.end(), front.begin(), front.end());
	}

	// Offset cube model by location within chunk
	for (unsigned int i = 0; i < meshPart.size() / vertexFloatCount; i++)
	{
		meshPart.at(i * vertexFloatCount + 0) += worldX;
		meshPart.at(i * vertexFloatCount + 1) += worldY;
		meshPart.at(i * vertexFloatCount + 2) += worldZ;
	}
}

bool Chunk::getFreeFace(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, CubeFace face)
{
	int32_t worldX = std::get<0>(worldCoords);
	int32_t worldY = std::get<1>(worldCoords);
	int32_t worldZ = std::get<2>(worldCoords);

	switch (face)
	{
	case CubeFace::Left:
		return !getBlockExists(std::make_tuple(worldX - 1, worldY, worldZ));
	case CubeFace::Right:
		return !getBlockExists(std::make_tuple(worldX + 1, worldY, worldZ));
	case CubeFace::Bottom:
		return !getBlockExists(std::make_tuple(worldX, worldY - 1, worldZ));
	case CubeFace::Top:
		return !getBlockExists(std::make_tuple(worldX, worldY + 1, worldZ));
	case CubeFace::Back:
		return !getBlockExists(std::make_tuple(worldX, worldY, worldZ - 1));
	case CubeFace::Front:
		return !getBlockExists(std::make_tuple(worldX, worldY, worldZ + 1));
	}

	return false;
}

void Chunk::chunkToWorldCoords(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, const std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords, std::tuple<int32_t, int32_t, int32_t>& worldCoords)
{
	int32_t chunkX = std::get<0>(chunkCoords);
	int32_t chunkY = std::get<1>(chunkCoords);
	int32_t chunkZ = std::get<2>(chunkCoords);

	uint32_t blockX = std::get<0>(blockCoords);
	uint32_t blockY = std::get<1>(blockCoords);
	uint32_t blockZ = std::get<2>(blockCoords);

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

bool Chunk::getBlockExists(const std::tuple<int32_t, int32_t, int32_t>& worldCoords)
{
	std::tuple<int32_t, int32_t, int32_t> chunkCoords;
	std::tuple<uint32_t, uint32_t, uint32_t> blockCoords;
	worldToChunkCoords(worldCoords, chunkCoords, blockCoords);

	int32_t chunkX = std::get<0>(chunkCoords);
	int32_t chunkY = std::get<1>(chunkCoords);
	int32_t chunkZ = std::get<2>(chunkCoords);

	uint32_t blockX = std::get<0>(blockCoords);
	uint32_t blockY = std::get<1>(blockCoords);
	uint32_t blockZ = std::get<2>(blockCoords);

	if (blocks.count(chunkCoords) == 0)
	{
		return false;
	}

	return blocks.at(chunkCoords).at(blockZ * intPow(chunkLength, 2) + blockY * intPow(chunkLength, 1) + blockX * intPow(chunkLength, 0)) != BlockType::Air;
}

int Chunk::intPow(int base, int exp)
{
	return static_cast<int>(std::powf(base, exp));
}

void Chunk::generateMesh(std::vector<float>& mesh, std::tuple<int32_t, int32_t, int32_t> chunkCoords)
{
	mesh.clear();
	for (unsigned int i = 0; i < blocks.at(chunkCoords).size(); i++)
	{
		BlockType blockType = blocks.at(chunkCoords).at(i);

		if (blockType == BlockType::Air)
		{
			continue;
		}

		uint32_t blockX = (i % intPow(chunkLength, 1)) / intPow(chunkLength, 0);
		uint32_t blockY = (i % intPow(chunkLength, 2)) / intPow(chunkLength, 1);
		uint32_t blockZ = (i % intPow(chunkLength, 3)) / intPow(chunkLength, 2);
		std::tuple<uint32_t, uint32_t, uint32_t> blockCoords = std::make_tuple(blockX, blockY, blockZ);

		std::tuple<int32_t, int32_t, int32_t> worldCoords;
		chunkToWorldCoords(chunkCoords, blockCoords, worldCoords);

		std::vector<float> meshPart;
		getFreeVertices(meshPart, worldCoords, blockType);
		
		mesh.insert(mesh.end(), meshPart.begin(), meshPart.end());
	}
}