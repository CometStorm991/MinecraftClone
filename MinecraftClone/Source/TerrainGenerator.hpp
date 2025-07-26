#pragma once

#include <cmath>
#include <cstdint>
#include <tuple>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BlockType.hpp"

class TerrainGenerator
{
public:
	TerrainGenerator(
		std::vector<BlockType>& chunkData,
		uint32_t chunkLength
	);

	void generateBlocks(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords);

	uint32_t getSolidBlockCount();
private:
	std::vector<BlockType>& chunkData;
	uint32_t chunkLength;
	uint32_t chunkGenLength;
	uint32_t solidBlockCount = 0;

	int32_t minGenerationY = 0;
	int32_t maxGenerationY = 63;

	const std::pair<float, float> gradientVectors[8] = {
		{1,0}, {-1,0}, {0,1}, {0,-1},
		{0.7071f, 0.7071f}, {-0.7071f, 0.7071f},
		{0.7071f, -0.7071f}, {-0.7071f, -0.7071f}
	};

	BlockType getTerrain(const std::tuple<int32_t, int32_t, int32_t>& worldCoords);
	bool getTerrainExists(const std::tuple<int32_t, int32_t, int32_t>& worldCoords);
	float perlinNoise(float x, float z);
	glm::vec2 generateGradient(int32_t x, int32_t z);
	uint32_t murmurHash2D(int32_t x, int32_t z, uint32_t seed = 2166136261u);
	float smoothStep(float lower, float upper, float value);
	void placeBlockInChunk(const std::tuple<int32_t, int32_t, int32_t>& blockCoords, BlockType blockType);

	void chunkToWorldCoords(const std::tuple<int32_t, int32_t, int32_t>& chunkCoords, const std::tuple<int32_t, int32_t, int32_t>& blockCoords, std::tuple<int32_t, int32_t, int32_t>& worldCoords);
	int32_t chunkToWorldCoord(int32_t chunkCoord, uint32_t blockCoord);
	void worldToChunkCoords(const std::tuple<int32_t, int32_t, int32_t>& worldCoords, std::tuple<int32_t, int32_t, int32_t>& chunkCoords, std::tuple<uint32_t, uint32_t, uint32_t>& blockCoords);
	std::tuple<int32_t, uint32_t> worldToChunkCoord(int32_t worldCoord);
	int32_t worldToBlockCoord(int32_t worldCoord, int32_t chunkCoord);

	int intPow(int base, int exp);
};