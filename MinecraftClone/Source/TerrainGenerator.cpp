#include "TerrainGenerator.hpp"

TerrainGenerator::TerrainGenerator(
	std::vector<BlockType>& chunkData,
	uint32_t chunkLength
) :
	chunkData(chunkData),
	chunkLength(chunkLength),
	chunkGenLength(chunkLength + 2)
{

}

void TerrainGenerator::generateBlocks(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords)
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

uint32_t TerrainGenerator::getSolidBlockCount()
{
	return solidBlockCount;
}

BlockType TerrainGenerator::getTerrain(const std::tuple<int32_t, int32_t, int32_t>& worldCoords)
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

bool TerrainGenerator::getTerrainExists(const std::tuple<int32_t, int32_t, int32_t>& worldCoords)
{
	int32_t worldX = std::get<0>(worldCoords);
	int32_t worldY = std::get<1>(worldCoords);
	int32_t worldZ = std::get<2>(worldCoords);

	/*if (worldY < minGenerationY || worldY > maxGenerationY)
	{
		return false;
	}*/

	// int32_t maxY = 8.0f * (std::sin(worldX / 8.0f) + std::sin(worldZ / 8.0f) + 2.0f);
	float normalizedPerlin = (perlinNoise(worldX / 80.0f, worldZ / 80.0f) + 1.0f) * 0.5f;
	int32_t maxY = normalizedPerlin * normalizedPerlin * 128.0f;
	/*maxY = std::min(maxY, maxGenerationY);
	maxY = std::max(maxY, minGenerationY);*/

	return worldY <= maxY;
}

float TerrainGenerator::perlinNoise(float x, float z)
{
	glm::vec2 position = glm::vec2(x, z);

	int32_t floorX = static_cast<int32_t>(std::floor(x));
	int32_t ceilX = static_cast<int32_t>(std::ceil(x));
	int32_t floorZ = static_cast<int32_t>(std::floor(z));
	int32_t ceilZ = static_cast<int32_t>(std::ceil(z));

	glm::vec2 lowerLeftToCenter = position - glm::vec2(floorX, floorZ);
	glm::vec2 lowerRightToCenter = position - glm::vec2(ceilX, floorZ);
	glm::vec2 upperLeftToCenter = position - glm::vec2(floorX, ceilZ);
	glm::vec2 upperRightToCenter = position - glm::vec2(ceilX, ceilZ);

	glm::vec2 lowerLeftGradient = generateGradient(floorX, floorZ);
	glm::vec2 lowerRightGradient = generateGradient(ceilX, floorZ);
	glm::vec2 upperLeftGradient = generateGradient(floorX, ceilZ);
	glm::vec2 upperRightGradient = generateGradient(ceilX, ceilZ);

	float lowerLeft = glm::dot(lowerLeftGradient, lowerLeftToCenter);
	float lowerRight = glm::dot(lowerRightGradient, lowerRightToCenter);
	float upperLeft = glm::dot(upperLeftGradient, upperLeftToCenter);
	float upperRight = glm::dot(upperRightGradient, upperRightToCenter);

	float xValue = ceilX - floorX == 0.0f ? 0.0f : (x - floorX) / (ceilX - floorX);
	float zValue = ceilZ - floorZ == 0.0f ? 0.0f : (z - floorZ) / (ceilZ - floorZ);

	float lower = smoothStep(lowerLeft, lowerRight, xValue);
	float upper = smoothStep(upperLeft, upperRight, xValue);

	return smoothStep(lower, upper, zValue);
}

float TerrainGenerator::smoothStep(float lower, float upper, float value)
{
	float actualValue = 3.0f * value * value - 2.0f * value * value * value;
	return lower + actualValue * (upper - lower);
}

// Generate a gradient in the form < [-1, 1], [-1, 1] >
glm::vec2 TerrainGenerator::generateGradient(int32_t x, int32_t z)
{
	uint32_t h = murmurHash2D(x, z);
	float gradientX = gradientVectors[h % 8].first;
	float gradientZ = gradientVectors[h % 8].second;

	return glm::vec2(gradientX, gradientZ);
}

uint32_t TerrainGenerator::murmurHash2D(int32_t x, int32_t z, uint32_t seed) {
	uint32_t h = 0x811C9DC5u;  // or any fixed seed
	h ^= static_cast<uint32_t>(x);
	h *= 0x5bd1e995;
	h ^= static_cast<uint32_t>(z);
	h *= 0x5bd1e995;
	h ^= h >> 13;
	return h;
}

void TerrainGenerator::placeBlockInChunk(const std::tuple<int32_t, int32_t, int32_t>& blockCoords, BlockType blockType)
{
	int32_t blockX = std::get<0>(blockCoords);
	int32_t blockY = std::get<1>(blockCoords);
	int32_t blockZ = std::get<2>(blockCoords);

	int32_t blockGenX = blockX + 1;
	int32_t blockGenY = blockY + 1;
	int32_t blockGenZ = blockZ + 1;

	chunkData.at(blockGenZ * intPow(chunkGenLength, 2) + blockGenY * intPow(chunkGenLength, 1) + blockGenX * intPow(chunkGenLength, 0)) = blockType;

	if (blockType != BlockType::Air)
	{
		solidBlockCount++;
	}
}

void TerrainGenerator::chunkToWorldCoords(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, const std::tuple<int32_t, int32_t, int32_t>& blockCoords, std::tuple<int32_t, int32_t, int32_t>& worldCoords)
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

int32_t TerrainGenerator::chunkToWorldCoord(int32_t chunkCoord, uint32_t blockCoord)
{
	return chunkCoord * chunkLength + blockCoord;
}

void TerrainGenerator::worldToChunkCoords(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, std::tuple<int32_t, int32_t, int32_t>& chunkCoords, std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords)
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

std::tuple<int32_t, uint32_t> TerrainGenerator::worldToChunkCoord(int32_t worldCoord)
{
	int32_t chunkCoord = worldCoord / chunkLength;
	uint32_t blockCoord = (worldCoord % chunkLength + chunkLength) % chunkLength;

	return std::make_tuple(chunkCoord, blockCoord);
}

int32_t TerrainGenerator::worldToBlockCoord(int32_t worldCoord, int32_t chunkCoord)
{
	return worldCoord - (chunkCoord * chunkLength);
}

int TerrainGenerator::intPow(int base, int exp)
{
	return static_cast<int>(std::pow(base, exp));
}